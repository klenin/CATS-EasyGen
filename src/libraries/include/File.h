#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__

FILE *FileOpen(const char *filename, const char *mode);
char *FileReadTextFile(const char *filename);

#endif // __UTILS_H__
