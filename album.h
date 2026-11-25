#ifndef ALBUM_H
#define ALBUM_H

#include "common.h"

/* ============================================================
 * album.h —— 组员 A：音乐资料库模块
 *
 * 签名严格按 interface_spec.md 2.1 节。
 * 这些是 D 的 main.c 和 B/C 会调用的“组内合同”，
 * 不要改函数名、参数顺序、参数类型、const。
 *
 * 数据所有权：
 *   albums[] / albumCount 由 D 的 runSystem 持有；
 *   A 通过指针修改，B/C/D 只读传入。
 * ============================================================ */

/* ---------- 启动加载与退出保存 ---------- */

/**
 * @brief 从 filename 读取专辑数据到 albums[]
 *
 * @param filename   数据文件路径，D 传 FILE_ALBUMS
 * @param albums     输出数组，调用方保证至少有 maxCount 个元素
 * @param maxCount   本次最多读取多少条；建议 D 传 MAX_ALBUMS
 * @param albumCount 输出：实际读到的有效条数；失败/文件不存在时置 0
 *
 * @return 实际读到的有效条数（>= 0）；文件不存在/打开失败返回 0
 *
 * 边界：
 *   - filename/albums/albumCount 为 NULL：不崩溃，*albumCount=0，返回 0
 *   - maxCount <= 0：*albumCount=0，返回 0
 *   - maxCount > MAX_ALBUMS：按 MAX_ALBUMS 处理
 *   - 空行跳过；格式错误/字段非法/编号重复的行跳过并打印警告
 *   - 超过容量：停止读取并打印警告
 *   - 严禁 exit()
 */
int loadAlbums(const char *filename, Album albums[], int maxCount, int *albumCount);

/**
 * @brief 将 albums[] 全部覆写到 filename
 *
 * @param filename   数据文件路径，D 传 FILE_ALBUMS
 * @param albums     只读数组
 * @param albumCount 有效条数（0 <= albumCount <= MAX_ALBUMS）
 *
 * @return 1 成功；0 失败
 *
 * 边界：
 *   - filename/albums 为 NULL、albumCount < 0 或 > MAX_ALBUMS：返回 0
 *   - albumCount == 0：创建空文件，返回 1（表示“空库”保存成功）
 *   - 写文件失败/关闭失败：返回 0
 *   - 不修改 albums[]
 */
int saveAlbums(const char *filename, const Album albums[], int albumCount);

/* ---------- 音乐资料库子菜单 1~8 ---------- */

/**
 * @brief 新增专辑（子菜单 1）
 *
 * @param albums     数组
 * @param albumCount 当前数量；成功后 (*albumCount)++
 * @param maxCount   容量上限；建议 D 传 MAX_ALBUMS
 *
 * 边界：
 *   - albums/albumCount 为 NULL：直接返回
 *   - maxCount <= 0 或 *albumCount >= maxCount 或 >= MAX_ALBUMS：提示已满，返回
 *   - id 必须为 1~MAX_ALBUM_ID 且全库唯一
 *   - title/artist 去首尾空白后非空、长度 < MAX_STR_LEN、不含 '|'
 *   - genre 0~GENRE_COUNT-1；mood 0~MOOD_COUNT-1；year MIN_YEAR~MAX_YEAR
 *   - 全部校验通过后才写入数组并 ++；失败不能留下半条记录
 */
void addAlbum(Album albums[], int *albumCount, int maxCount);

/**
 * @brief 按 id 删除专辑（子菜单 2）
 *
 * @param albums     数组
 * @param albumCount 当前数量；成功后 (*albumCount)--
 *
 * 边界：
 *   - albums/albumCount 为 NULL 或 *albumCount <= 0：提示空库，返回
 *   - id 不存在：提示，返回
 *   - 删除后后面的元素前移，数量减一
 */
void deleteAlbum(Album albums[], int *albumCount);

/**
 * @brief 按 id 修改专辑（子菜单 3）
 *
 * @param albums     数组
 * @param albumCount 当前数量（不修改数量）
 *
 * 边界：
 *   - albums 为 NULL 或 albumCount <= 0：提示空库，返回
 *   - id 不存在：提示，返回
 *   - 只允许修改 title/artist/genre/mood/year；id 保持不变
 *   - 每一项直接回车表示“保持不变”
 *   - 修改后的数据必须通过校验；失败时原记录不变（先改副本）
 */
void modifyAlbum(Album albums[], int albumCount);

/**
 * @brief 显示全部专辑（子菜单 4）
 *
 * @param albums     只读数组
 * @param albumCount 有效条数
 *
 * 边界：
 *   - albums 为 NULL 或 albumCount <= 0：显示“当前音乐资料库为空。”
 *   - 表格至少包含：ID、名称、歌手、风格名、情绪名、年份
 *   - 不修改数组
 */
void showAllAlbums(const Album albums[], int albumCount);

/**
 * @brief 查询专辑（子菜单 5）
 *
 * @param albums     只读数组
 * @param albumCount 有效条数
 *
 * 控制台子菜单：
 *   1. 按 ID 精确查询
 *   2. 按名称部分匹配（strstr）
 *   3. 按歌手部分匹配（strstr）
 *   0. 返回
 *
 * 边界：
 *   - 空库：提示，返回
 *   - 关键字为空：提示，返回
 *   - 未命中：提示
 *   - 多个命中：全部列出
 *   - 名称/歌手匹配区分大小写（strstr 的默认行为）
 */
void searchAlbum(const Album albums[], int albumCount);

/**
 * @brief 按风格筛选（子菜单 6）
 *
 * @param albums     只读数组
 * @param albumCount 有效条数
 *
 * 边界：
 *   - 空库：提示，返回
 *   - 先打印 0~GENRE_COUNT-1 的风格对照表，再读编号
 *   - 输入必须 0~GENRE_COUNT-1
 *   - 无匹配：提示
 */
void filterAlbumsByGenre(const Album albums[], int albumCount);

/**
 * @brief 专辑排序（子菜单 7）
 *
 * @param albums     数组（原地排序）
 * @param albumCount 有效条数
 *
 * 控制台子菜单：
 *   排序字段：1. ID  2. 发行年份  3. 名称
 *   排序方向：1. 升序  2. 降序
 *
 * 边界：
 *   - 空库：提示，返回
 *   - 只有 1 条：提示无需排序（或直接展示）
 *   - 必须交换整个 Album 结构体，不能只交换某个字段
 *   - 排序后展示结果
 *   - 名称比较用 strcmp（按字节，不按拼音）
 */
void sortAlbums(Album albums[], int albumCount);

/**
 * @brief 音乐资料统计（子菜单 8）
 *
 * @param albums     只读数组
 * @param albumCount 有效条数
 *
 * 输出：
 *   - 专辑总数
 *   - 各风格数量（0~GENRE_COUNT-1，包括 0 张的风格）
 *   - 各情绪数量（0~MOOD_COUNT-1，包括 0 张的情绪）
 *   - 可选：最早/最晚发行年份的专辑
 *
 * 边界：
 *   - 空库：提示，返回
 *   - 统计数组必须初始化为 0
 *   - genre/mood 越界的数据不应导致数组越界
 */
void showAlbumStats(const Album albums[], int albumCount);

/* ---------- 跨模块公共辅助函数（B/C/D 可调用） ---------- */

/**
 * @brief 按专辑 ID 查找下标
 *
 * @param albums     只读数组
 * @param albumCount 有效条数
 * @param albumId    要查找的 ID
 *
 * @return 找到返回下标（>= 0）；未找到/参数非法返回 -1
 */
int findAlbumIndexById(const Album albums[], int albumCount, int albumId);

/**
 * @brief 风格编号转中文名
 *
 * @param genre 风格编号
 * @return 0~10 对应中文名；GENRE_OTHER 返回“其他”；越界返回“未知”
 * @note 返回值是字符串常量，调用方不要 free/修改
 */
const char *getGenreName(int genre);

/**
 * @brief 情绪编号转中文名
 *
 * @param mood 情绪编号
 * @return 0~4 对应中文名；越界返回“未知”
 * @note 返回值是字符串常量，调用方不要 free/修改
 */
const char *getMoodName(int mood);

#endif /* ALBUM_H */
