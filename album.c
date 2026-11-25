#include <stddef.h>
#include "album.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const char *getGenreName(int genre)
{
    static const char *names[GENRE_COUNT] = {
        "娴佽", "鎽囨粴", "鐖靛＋", "鍙ゅ吀", "鐢靛瓙",
        "鍢诲搱", "R&B", "姘戣埃", "閲戝睘", "褰辫鍘熷０", "鍏朵粬"
    };
    if (genre < 0 || genre >= GENRE_COUNT) {
        return "鏈煡";
    }
    return names[genre];
}

const char *getMoodName(int mood)
{
    static const char *names[MOOD_COUNT] = {
        "娆㈠揩", "浼ゆ劅", "鏀炬澗", "鍏呮弧娲诲姏", "娴极"
    };
    if (mood < 0 || mood >= MOOD_COUNT) {
        return "鏈煡";
    }
    return names[mood];
}

int findAlbumIndexById(const Album albums[], int albumCount, int albumId)
{
    int i;

    if (albums == NULL || albumCount <= 0) {
        return -1;
    }
    for (i = 0; i < albumCount; i++) {
        if (albums[i].id == albumId) {
            return i;
        }
    }
    return -1;
}

void showAllAlbums(const Album albums[], int albumCount)
{
	if(albums == NULL || albumCount <=0)
	{
		printf("当前音乐资料库为空\n");
		return ; 
	}
    printf("编号 | 专辑名称 | 艺术家 | 风格 | 情绪 | 年份\n");
    printf("-----+----------+--------+------+------+------\n");
    for(int i = 0;i<albumCount;i++)
    {
    printf("%d | %s | %s | %s | %s | %d\n",
    albums[i].id,
    albums[i].title,
    albums[i].artist,
    getGenreName(albums[i].genre),
    getMoodName(albums[i].mood),
    albums[i].year);
	}
	printf("共%d张专辑\n",albumCount);
}

void deleteAlbum(Album albums[], int *albumCount)
{
	if (albums == NULL || albumCount == NULL) return;
	if(*albumCount<=0)
	{
		printf("当前音乐资料库为空\n");
		return ; 
	}
	int id;
	int ch;
	printf("请输入要删除的专辑编号：");
	if(scanf("%d",&id) !=1)
	{
		while((ch=getchar()) !='\n' && ch != EOF){}
		printf("编号无效\n");
		return;
	}
	while((ch=getchar()) !='\n' && ch != EOF){}
	int index = findAlbumIndexById(albums, *albumCount, id);
	if(index<0)
	{
		printf("未找到编号为%d的专辑\n",id);
		return;
	}
	printf("将要删除:%s - %s\n",albums[index].title,albums[index].artist);
	for(int i = index;i<*albumCount-1;i++)
	{
		albums[i] = albums[i+1];
	}
	(*albumCount)--;
	printf("删除成功,现在共有%d张专辑\n",*albumCount);
}

void addAlbum(Album albums[], int *albumCount, int maxCount)
{
	
}


















