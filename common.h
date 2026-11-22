#ifndef COMMON_H
#define COMMON_H

/* ==================== 全局常量与文件路径 ==================== */
#define MAX_ALBUMS      500     // 专辑数组最大容量
#define MAX_RATINGS     1000    // 用户行为记录数组最大容量
#define TOP_RECOMMEND   5       // 默认推荐结果数量上限（Top 5）
#define MAX_STR_LEN     128      // 字符串字段最大长度（含结尾 '\0'）

#define FILE_ALBUMS     "data/albums.txt"   // 专辑数据文件路径（组员 A 读写）
#define FILE_RATINGS    "data/ratings.txt"  // 用户行为文件路径（组员 B 读写）
#define FILE_CONFIG     "data/config.txt"   // 推荐权重文件路径（组员 D 读写）

/* 默认推荐权重 */
#define DEFAULT_GENRE_WEIGHT  0.7
#define DEFAULT_MOOD_WEIGHT   0.3
/* ==================== 统一枚举定义 ==================== */

/**
 * @brief 音乐风格枚举
 * @note  存入文件或结构体时均使用对应的整数值（0 ~ GENRE_COUNT-1）
 */
typedef enum {
    GENRE_POP = 0,      // 0: 流行
    GENRE_ROCK,         // 1: 摇滚
    GENRE_JAZZ,         // 2: 爵士
    GENRE_CLASSICAL,    // 3: 古典
    GENRE_ELECTRONIC,   // 4: 电子
    GENRE_HIPHOP, 
    GENRE_RNB,
    GENRE_FOLK,
    GENRE_METAL,
    GENRE_SOUNDTRACK,      // 5: 嘻哈
    GENRE_COUNT         // 风格总数（哨兵值，用于循环边界和数组大小，非有效风格）
} Genre;

/**
 * @brief 音乐情绪枚举
 * @note  存入文件或结构体时均使用对应的整数值（0 ~ MOOD_COUNT-1）
 */
typedef enum {
    MOOD_HAPPY = 0,     // 0: 欢快
    MOOD_SAD,           // 1: 伤感
    MOOD_RELAXED,       // 2: 放松
    MOOD_ENERGETIC,     // 3: 燃 / 充满活力
    MOOD_ROMANTIC,      // 4: 浪漫
    MOOD_COUNT          // 情绪总数（哨兵值，非有效情绪）
} Mood;

/* ==================== 公共数据结构 ==================== */

/**
 * @brief 专辑信息结构体（由组员 A 维护增删改查）
 */
typedef struct {
    int id;                     // 专辑唯一编号（正整数，不可重复）
    char title[MAX_STR_LEN];    // 专辑名称（建议不含空格或读写时做分隔符处理）
    char artist[MAX_STR_LEN];   // 歌手 / 乐队名称
    int genre;                  // 风格编号，取值范围：0 ~ GENRE_COUNT-1
    int mood;                   // 情绪编号，取值范围：0 ~ MOOD_COUNT-1
    int year;                   // 发行年份（如 2024）
} Album;

/**
 * @brief 用户行为记录结构体（由组员 B 维护，每张有交互的专辑对应一条记录）
 */
typedef struct {
    int albumId;        // 关联的专辑 ID（对应 Album.id）
    int score;          // 用户评分：0 表示未评分，1~5 表示有效评分
    int feedback;       // 推荐反馈：-1 不感兴趣，0 无反馈，1 喜欢
    int isFavorite;     // 收藏状态：0 未收藏，1 已收藏
    int listened;       // 收听状态：0 未听，1 已听
} Rating;

/**
 * @brief 用户偏好画像结构体（由组员 B 计算生成，组员 C 读取用于推荐）
 */
typedef struct {
    double genrePref[GENRE_COUNT];  // 各风格的偏好得分（建议归一化到 0.0~1.0）
    double moodPref[MOOD_COUNT];    // 各情绪的偏好得分（建议归一化到 0.0~1.0）
} UserProfile;

/**
 * @brief 单条推荐结果结构体（由组员 C 计算生成）
 */
typedef struct {
    int albumId;        // 被推荐的专辑 ID
    double genreScore;  // 风格匹配分
    double moodScore;   // 情绪匹配分
    double totalScore;  // 综合评分 = genreScore * genreWeight + moodScore * moodWeight
} RecommendResult;

/* ==================== 控制台输入防坑约定 ==================== */
/*
 * 重要提示：
 * 1. 所有模块函数若在内部使用了 scanf()，在函数结束返回 main.c 之前，
 *    必须清空标准输入缓冲区中的残留换行符，否则会导致主菜单循环读取异常！
 *    推荐写法：int ch; while ((ch = getchar()) != '\n' && ch != EOF);
 * 2. 所有 load 函数在文件不存在时禁止调用 exit()，应将 *count 置 0 并返回 0。
 *
 * 3. genre 和 mood 的枚举值在文件中存储为整数，读取时需检查范围有效性（0 ~ GENRE_COUNT-1 或 0 ~ MOOD_COUNT-1）。
 *
 * 4. genrePref 和 moodPref 的偏好得分在计算时应归一化到 0.0~1.0 范围，避免出现负值或超过 1.0 的情况。
 *
 * 5. B组计算用户画像时：
 *    有有效偏好数据，hasData = 1，若无有效数据，hasData = 0。若 hasData = 0，则 C组在推荐时应使用默认权重（DEFAULT_GENRE_WEIGHT 和 DEFAULT_MOOD_WEIGHT）。

#endif
