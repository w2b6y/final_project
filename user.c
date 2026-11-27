#include "user.h"
#include "album.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Read one complete line. An oversized line is discarded as a unit. */
static int readLine(FILE *stream, char *line, int size)
{
    int ch, used = 0, invalid = 0;
    while ((ch = fgetc(stream)) != EOF && ch != '\n') {
        if (ch == '\0') invalid = 1;
        if (used < size - 1) line[used++] = (char)ch;
        else invalid = 1;
    }
    line[used] = '\0';
    if (ch == EOF && used == 0 && !invalid) return 0;
    return invalid ? -1 : 1;
}

static int parseInt(const char **cursor, int *value)
{
    char *end;
    long number;
    const char *p = *cursor;
    while (isspace((unsigned char)*p)) ++p;
    errno = 0;
    number = strtol(p, &end, 10);
    if (end == p || errno == ERANGE || number < INT_MIN || number > INT_MAX) return 0;
    *value = (int)number;
    *cursor = end;
    return 1;
}

static int readInt(const char *prompt, int min, int max, int *value)
{
    char line[256];
    const char *p;
    int status;
    for (;;) {
        fputs(prompt, stdout);
        fflush(stdout);
        status = readLine(stdin, line, sizeof line);
        if (!status) return 0;
        p = line;
        if (status > 0 && parseInt(&p, value)) {
            while (isspace((unsigned char)*p)) ++p;
            if (!*p && *value >= min && *value <= max) return 1;
        }
        printf("输入无效，请输入 %d 到 %d 的整数。\n", min, max);
    }
}

static int validRating(const Rating *r)
{
    return r->albumId > 0 && r->score >= 0 && r->score <= 5 &&
           r->feedback >= -1 && r->feedback <= 1 &&
           (r->isFavorite == 0 || r->isFavorite == 1) &&
           (r->listened == 0 || r->listened == 1);
}

int findRatingIndexByAlbumId(const Rating ratings[], int ratingCount, int albumId)
{
    int i;
    if (ratings) for (i = 0; i < ratingCount; ++i)
        if (ratings[i].albumId == albumId) return i;
    return -1;
}

/* Accept current whitespace format and the earlier, pipe-separated format. */
static int parseRating(const char *line, Rating *r)
{
    int fields[5], i, pipeFormat = strchr(line, '|') != NULL;
    const char *p = line;
    for (i = 0; i < 5; ++i) {
        if (!parseInt(&p, &fields[i])) return 0;
        if (i < 4) {
            if (!pipeFormat && !isspace((unsigned char)*p)) return 0;
            while (isspace((unsigned char)*p)) ++p;
            if (pipeFormat && *p++ != '|') return 0;
        }
    }
    while (isspace((unsigned char)*p)) ++p;
    if (*p) return 0;
    *r = (Rating){fields[0], fields[1], fields[2], fields[3], fields[4]};
    return validRating(r);
}

int loadRatings(const char *filename, Rating ratings[], int maxCount, int *ratingCount)
{
    FILE *file;
    char line[256];
    int status, index, firstLine = 1;
    Rating r;
    if (!ratingCount) return 0;
    *ratingCount = 0;
    if (!filename || !ratings || maxCount <= 0 || !(file = fopen(filename, "r"))) return 0;
    while ((status = readLine(file, line, sizeof line)) != 0) {
        const char *p = line;
        if (firstLine && strncmp(p, "\xEF\xBB\xBF", 3) == 0) p += 3;
        firstLine = 0;
        if (status < 0 || !parseRating(p, &r)) continue;
        index = findRatingIndexByAlbumId(ratings, *ratingCount, r.albumId);
        if (index >= 0) ratings[index] = r;
        else if (*ratingCount < maxCount) ratings[(*ratingCount)++] = r;
    }
    fclose(file);
    return *ratingCount;
}

int saveRatings(const char *filename, const Rating ratings[], int ratingCount)
{
    FILE *file;
    int i, ok = 1;
    if (!filename || ratingCount < 0 || (!ratings && ratingCount > 0)) return 0;
    for (i = 0; i < ratingCount; ++i)
        if (!validRating(&ratings[i]) ||
            findRatingIndexByAlbumId(ratings, i, ratings[i].albumId) >= 0) return 0;
    file = fopen(filename, "w");
    if (!file) return 0;
    for (i = 0; i < ratingCount; ++i) {
        const Rating *r = &ratings[i];
        if (fprintf(file, "%d %d %d %d %d\n", r->albumId, r->score,
                    r->feedback, r->isFavorite, r->listened) < 0) { ok = 0; break; }
    }
    if (fclose(file) != 0) ok = 0;
    return ok;
}

/* Validate all input before creating a record, including cancellation at EOF. */
static void editBehavior(const Album albums[], int albumCount,
                         Rating ratings[], int *ratingCount, int maxCount, int action)
{
    int albumId, index, value = 0;
    Rating *r;
    if (!ratings || !ratingCount || *ratingCount < 0 || *ratingCount > maxCount) {
        puts("用户行为数组或数量无效。"); return;
    }
    if (!albums || albumCount <= 0) { puts("音乐资料库为空。"); return; }
    if (!readInt("请输入专辑 ID：", 1, INT_MAX, &albumId)) return;
    if (findAlbumIndexById(albums, albumCount, albumId) < 0) {
        puts("专辑不存在，操作未保存。"); return;
    }
    index = findRatingIndexByAlbumId(ratings, *ratingCount, albumId);
    if (index < 0 && *ratingCount >= maxCount) { puts("用户行为记录已满。"); return; }
    if (action == 0 && !readInt("评分（1~5）：", 1, 5, &value)) return;
    if (action == 2 && !readInt("已听状态（0 未听，1 已听）：", 0, 1, &value)) return;
    if (action == 3 && !readInt("推荐反馈（-1 不感兴趣，0 无反馈，1 喜欢）：", -1, 1, &value)) return;
    if (index < 0) {
        index = (*ratingCount)++;
        ratings[index] = (Rating){albumId, 0, 0, 0, 0};
    }
    r = &ratings[index];
    if (action == 0) r->score = value;
    else if (action == 1) r->isFavorite = !r->isFavorite;
    else if (action == 2) r->listened = value;
    else r->feedback = value;
    puts(action == 1 ? (r->isFavorite ? "收藏成功。" : "已取消收藏。") : "更新成功。");
}

void rateAlbum(const Album a[], int n, Rating r[], int *c, int max)
{ editBehavior(a, n, r, c, max, 0); }
void toggleFavorite(const Album a[], int n, Rating r[], int *c, int max)
{ editBehavior(a, n, r, c, max, 1); }
void updateListenedStatus(const Album a[], int n, Rating r[], int *c, int max)
{ editBehavior(a, n, r, c, max, 2); }
void recordFeedback(const Album a[], int n, Rating r[], int *c, int max)
{ editBehavior(a, n, r, c, max, 3); }

static void printRecords(const Album albums[], int albumCount,
                         const Rating ratings[], int ratingCount, int filter, int id)
{
    int i, index, found = 0;
    puts("ID | 专辑名称 | 评分(0=未评) | 收藏 | 已听 | 推荐反馈");
    for (i = 0; ratings && i < ratingCount; ++i) {
        const Rating *r = &ratings[i];
        if ((filter == 1 && r->score == 0) || (filter == 2 && !r->isFavorite) ||
            (filter == 3 && !r->listened) || (filter == 4 && r->albumId != id)) continue;
        index = albums ? findAlbumIndexById(albums, albumCount, r->albumId) : -1;
        printf("%d | %s | %d | %s | %s | %s\n", r->albumId,
               index < 0 ? "[专辑已删除]" : albums[index].title, r->score,
               r->isFavorite ? "是" : "否", r->listened ? "是" : "否",
               r->feedback > 0 ? "喜欢" : r->feedback < 0 ? "不感兴趣" : "无反馈");
        ++found;
    }
    if (!found) puts("暂无符合条件的用户行为记录。");
}

void showRatings(const Album albums[], int albumCount, const Rating ratings[], int ratingCount)
{
    int filter, id = 0;
    printRecords(albums, albumCount, ratings, ratingCount, 0, 0);
    if (!ratings || ratingCount <= 0) return;
    while (readInt("查询：1 已评分 / 2 收藏 / 3 已听 / 4 按ID / 0 返回：", 0, 4, &filter) && filter) {
        if (filter == 4 && !readInt("请输入专辑 ID：", 1, INT_MAX, &id)) return;
        printRecords(albums, albumCount, ratings, ratingCount, filter, id);
    }
}

void getRatingStats(const Rating ratings[], int ratingCount,
                    int *ratedCount, int *favoriteCount, int *listenedCount)
{
    int i, rated = 0, favorite = 0, listened = 0;
    for (i = 0; ratings && i < ratingCount; ++i) {
        rated += ratings[i].score > 0;
        favorite += ratings[i].isFavorite == 1;
        listened += ratings[i].listened == 1;
    }
    if (ratedCount) *ratedCount = rated;
    if (favoriteCount) *favoriteCount = favorite;
    if (listenedCount) *listenedCount = listened;
}

/* A zero preference from an explicit dislike is data, rather than cold start. */
void buildUserProfile(const Album albums[], int albumCount,
                      const Rating ratings[], int ratingCount, UserProfile *profile)
{
    int genreCount[GENRE_COUNT] = {0}, moodCount[MOOD_COUNT] = {0};
    int i, index;
    if (!profile) return;
    memset(profile, 0, sizeof *profile);
    for (i = 0; albums && ratings && i < ratingCount; ++i) {
        const Rating *r = &ratings[i];
        const Album *a;
        double value;
        if (!validRating(r) || !(r->score || r->feedback || r->isFavorite || r->listened)) continue;
        index = findAlbumIndexById(albums, albumCount, r->albumId);
        if (index < 0) continue;
        a = &albums[index];
        if (a->genre < 0 || a->genre >= GENRE_COUNT || a->mood < 0 || a->mood >= MOOD_COUNT) continue;
        value = (r->score > 0 ? (r->score - 1) / 4.0 : 0.0) +
                0.20 * r->isFavorite + 0.05 * r->listened + 0.25 * r->feedback;
        if (value < 0.0) value = 0.0;
        if (value > 1.0) value = 1.0;
        profile->genrePref[a->genre] += value;
        profile->moodPref[a->mood] += value;
        ++genreCount[a->genre];
        ++moodCount[a->mood];
        profile->hasData = 1;
    }
    for (i = 0; i < GENRE_COUNT; ++i)
        if (genreCount[i]) profile->genrePref[i] /= genreCount[i];
    for (i = 0; i < MOOD_COUNT; ++i)
        if (moodCount[i]) profile->moodPref[i] /= moodCount[i];
}

/* Raw score averages complement the normalized, behavior-based preferences. */
static void showCategoryStats(const Album albums[], int albumCount,
                              const Rating ratings[], int ratingCount, int mood)
{
    double sums[GENRE_COUNT] = {0};
    int counts[GENRE_COUNT] = {0}, i, index, category, size = mood ? MOOD_COUNT : GENRE_COUNT;
    for (i = 0; albums && ratings && i < ratingCount; ++i) {
        if (!validRating(&ratings[i]) || !ratings[i].score) continue;
        index = findAlbumIndexById(albums, albumCount, ratings[i].albumId);
        if (index < 0) continue;
        category = mood ? albums[index].mood : albums[index].genre;
        if (category < 0 || category >= size) continue;
        sums[category] += ratings[i].score;
        ++counts[category];
    }
    for (i = 0; i < size; ++i) {
        printf("%s：", mood ? getMoodName(i) : getGenreName(i));
        if (counts[i]) printf("平均 %.2f 分（%d 张）\n", sums[i] / counts[i], counts[i]);
        else puts("暂无评分");
    }
}

static void showPreferences(const double values[], int count, int mood)
{
    double best = 0.0;
    int i;
    for (i = 0; i < count; ++i) {
        printf("%s：%.3f\n", mood ? getMoodName(i) : getGenreName(i), values[i]);
        if (values[i] > best) best = values[i];
    }
    printf("最偏好的%s：", mood ? "情绪" : "风格");
    if (best == 0.0) puts("暂无正向偏好");
    else {
        for (i = 0; i < count; ++i)
            if (values[i] >= best - 1e-9) printf("%s ", mood ? getMoodName(i) : getGenreName(i));
        putchar('\n');
    }
}

void showUserProfile(const Album albums[], int albumCount,
                     const Rating ratings[], int ratingCount, UserProfile *profile)
{
    int rated, favorite, listened;
    if (!profile) return;
    buildUserProfile(albums, albumCount, ratings, ratingCount, profile);
    getRatingStats(ratings, ratingCount, &rated, &favorite, &listened);
    printf("已评分：%d，收藏：%d，已听：%d\n", rated, favorite, listened);
    if (!profile->hasData) { puts("暂无足够偏好数据。"); return; }
    puts("风格偏好（0~1）：");
    showPreferences(profile->genrePref, GENRE_COUNT, 0);
    puts("情绪偏好（0~1）：");
    showPreferences(profile->moodPref, MOOD_COUNT, 1);
    puts("各风格平均评分：");
    showCategoryStats(albums, albumCount, ratings, ratingCount, 0);
    puts("各情绪平均评分：");
    showCategoryStats(albums, albumCount, ratings, ratingCount, 1);
}
