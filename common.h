#ifndef COMMON_H
#define COMMON_H

/* ==================== 全局常量与文件路径 ==================== */
#define MAX_ALBUMS      500
#define MAX_RATINGS     1000
#define TOP_RECOMMEND   5
#define MAX_STR_LEN     128

#define FILE_ALBUMS     "data/albums.txt"
#define FILE_RATINGS    "data/ratings.txt"
#define FILE_CONFIG     "data/config.txt"

/* 默认推荐权重 */
#define DEFAULT_GENRE_WEIGHT  0.7
#define DEFAULT_MOOD_WEIGHT   0.3

/* ==================== 音乐风格 ==================== */
typedef enum {
    GENRE_POP = 0,       // 流行
    GENRE_ROCK,          // 摇滚
    GENRE_JAZZ,          // 爵士
    GENRE_CLASSICAL,     // 古典
    GENRE_ELECTRONIC,    // 电子
    GENRE_HIPHOP,        // 嘻哈
    GENRE_RNB,           // R&B
    GENRE_FOLK,          // 民谣
    GENRE_METAL,         // 金属
    GENRE_SOUNDTRACK,    // 影视原声
    GENRE_OTHER,         // 其他
    GENRE_COUNT
} Genre;

/* ==================== 音乐情绪 ==================== */
typedef enum {
    MOOD_HAPPY = 0,      // 欢快
    MOOD_SAD,            // 伤感
    MOOD_RELAXED,        // 放松
    MOOD_ENERGETIC,      // 充满活力
    MOOD_ROMANTIC,       // 浪漫
    MOOD_COUNT
} Mood;

/* ==================== 专辑信息 ==================== */
typedef struct {
    int id;
    char title[MAX_STR_LEN];
    char artist[MAX_STR_LEN];
    int genre;           // 取值范围：0 ~ GENRE_COUNT-1
    int mood;            // 取值范围：0 ~ MOOD_COUNT-1
    int year;
} Album;

/* ==================== 用户行为记录 ==================== */
typedef struct {
    int albumId;
    int score;           // 0：未评分，1~5：评分
    int feedback;        // -1：不感兴趣，0：无反馈，1：喜欢
    int isFavorite;      // 0：未收藏，1：已收藏
    int listened;        // 0：未听，1：已听
} Rating;

/* ==================== 用户偏好画像 ==================== */
typedef struct {
    double genrePref[GENRE_COUNT];
    double moodPref[MOOD_COUNT];
    int hasData;         // 1：有有效偏好数据，0：暂无有效偏好数据
} UserProfile;

/* ==================== 推荐结果 ==================== */
typedef struct {
    int albumId;
    double genreScore;
    double moodScore;
    double totalScore;
} RecommendResult;

/* ==================== 输入与文件处理约定 ==================== */
/*
 * 1. 函数内部使用 scanf() 后，应处理残留换行符，
 *    避免影响主菜单后续输入。
 *
 * 2. load 函数在文件不存在时不得调用 exit()。
 *    应将数量置为 0，并按照团队约定返回结果。
 *
 * 3. genre、mood 必须使用上方枚举对应的编号。
 *
 * 4. genrePref、moodPref 的取值范围为 0.0~1.0。
 *
 * 5. B 组计算用户画像时：
 *    有有效偏好数据，hasData = 1；
 *    没有有效偏好数据，hasData = 0。
 */

#endif
