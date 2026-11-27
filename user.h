#ifndef USER_H
#define USER_H

#include "common.h"

int loadRatings(const char *filename, Rating ratings[], int maxCount, int *ratingCount);
int saveRatings(const char *filename, const Rating ratings[], int ratingCount);

void rateAlbum(const Album albums[], int albumCount,
               Rating ratings[], int *ratingCount, int maxCount);
void toggleFavorite(const Album albums[], int albumCount,
                    Rating ratings[], int *ratingCount, int maxCount);
void updateListenedStatus(const Album albums[], int albumCount,
                          Rating ratings[], int *ratingCount, int maxCount);
void recordFeedback(const Album albums[], int albumCount,
                    Rating ratings[], int *ratingCount, int maxCount);
void showRatings(const Album albums[], int albumCount,
                 const Rating ratings[], int ratingCount);
void showUserProfile(const Album albums[], int albumCount,
                     const Rating ratings[], int ratingCount, UserProfile *profile);

int findRatingIndexByAlbumId(const Rating ratings[], int ratingCount, int albumId);
void buildUserProfile(const Album albums[], int albumCount,
                      const Rating ratings[], int ratingCount, UserProfile *profile);
void getRatingStats(const Rating ratings[], int ratingCount,
                    int *ratedCount, int *favoriteCount, int *listenedCount);

#endif
