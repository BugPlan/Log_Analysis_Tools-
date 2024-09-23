#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#include <windows.h>
#include <sys/stat.h>  
#include <direct.h>
#define MAX_PATH 260 


//遍历当前文件夹函数
void TraverseDirectory(const char* dir) {  
    HANDLE hFind;  
    WIN32_FIND_DATA data;  
    char path[MAX_PATH];  
    char fullPath[MAX_PATH];  
  
    sprintf(path, "%s\\*", dir);  
  
    hFind = FindFirstFile(path, &data);  
    if (hFind == INVALID_HANDLE_VALUE) {  
        printf("Failed to list directory contents for '%s'.\n", dir);  
        return;  
    }  
  
    do {  
        if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)  
            continue;  
  
        sprintf(fullPath, "%s\\%s", dir, data.cFileName);  
  
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  
            printf("Directory: %s\n", fullPath);  
            // 递归遍历目录  
            TraverseDirectory(fullPath);  
        } else {  
            printf("File: %s\n", fullPath);  
        }  
    } while (FindNextFile(hFind, &data) != 0);  
  
    if (GetLastError() != ERROR_NO_MORE_FILES) {  
        printf("FindNextFile failed (%d)\n", GetLastError());  
    }  
  
    FindClose(hFind);  
}  

// 日期解析函数   解析格式为 YYYY-MM-DD  
int parse_date(const char* date_str, int* year, int* month, int* day) {  
    if (sscanf(date_str, "%d-%d-%d", year, month, day) != 3) {  
        return 0; // 解析失败  
    }  
    return 1; // 解析成功  
} 
  
// 日期范围比较函数
int is_date_in_range(const char* date_str, const char* start_date, const char* end_date) {  
    int year, month, day;  
    int start_year, start_month, start_day;  
    int end_year, end_month, end_day;  
  
    if (!parse_date(date_str, &year, &month, &day) ||  
        !parse_date(start_date, &start_year, &start_month, &start_day) ||  
        !parse_date(end_date, &end_year, &end_month, &end_day)) {  
        return 0; // 解析失败  
    }  
  
    // 检查是否在日期范围内  
    if (year > end_year || (year == end_year && month > end_month) ||   
        (year == end_year && month == end_month && day > end_day)) {
        return 0; // 日期超出范围  
    }  
    if (year < start_year || (year == start_year && month < start_month) ||   
        (year == start_year && month == start_month && day < start_day)) {  
        return 0; // 日期在范围之前  
    }  
  
    return 1; // 日期在范围内  
}
  
// 简化的日期处理，暂时返回true以跳过日期检查  
/*int is_date_in_range(const char* date_str, const char* start_date, const char* end_date) {  
    return 1; // 总是返回true，以跳过日期检查  
}*/

// 日志搜索函数   
void search_keywords(const char* filename, const char* keywords) {  
    FILE *file = fopen(filename, "r");  
    if (!file) {  
        perror("Failed to open file");  
        return;  
    }  
  
    char buffer[1024];  
    while (fgets(buffer, sizeof(buffer), file)) {  
        if (strstr(buffer, keywords)) {  
            printf("%s", buffer);  
        }  
    }  
  
    fclose(file);  
}  
  
// 关键字计数函数  
void count_keywords(const char* filename, const char* keywords) {  
    FILE *file = fopen(filename, "r");  
    if (!file) {  
        perror("Failed to open file");  
        return;  
    }  
  
    int count = 0;  
    char buffer[1024];  
    while (fgets(buffer, sizeof(buffer), file)) {  
        if (strstr(buffer, keywords)) {  
            count++;  
        }  
    }  
  
    printf("Keyword '%s' appears %d times.\n", keywords, count);  
  
    fclose(file);  
}  

//日期范围和关键字过滤函数
void filter_by_date_range_and_keyword(const char* filename, const char* start_date, const char* end_date, const char* keyword) {  
    FILE *file = fopen(filename, "r");  
    if (!file) {  
        perror("Failed to open file");  
        return;  
    }  
  
    char buffer[1024];  
    while (fgets(buffer, sizeof(buffer), file)) {  
        // 日志的第一部分是日期  
        char date_str[20];  
        sscanf(buffer, "%19[^ ]", date_str); // 读取直到空格的字符串 
  
        if (is_date_in_range(date_str, start_date, end_date)) {  
            if (keyword == NULL || strstr(buffer, keyword)) {  
                printf("%s", buffer);  
            }  
        }  
    }  
  
    fclose(file);  
}    
///!!!!改进日期范围和关键字过滤函数
void new_filter_by_date_range_and_keyword(const char* filename, const char* start_date, const char* end_date, const char* keyword) {  
    FILE *file = fopen(filename, "r");  
    if (!file) {  
        perror("Failed to open file");  
        return;  
    }  
  
    char buffer[1024];  
    while (fgets(buffer, sizeof(buffer), file)) {  
        char date_str[20];  
        sscanf(buffer, "%19[^ ]", date_str);  
  
        int isInRange = 1; // 默认在范围内  
        if (start_date && end_date) {  
            isInRange = is_date_in_range(date_str, start_date, end_date);  
        } else if (start_date) {  
            isInRange = (strcmp(date_str, start_date) >= 0); // 仅检查开始日期  
        } else if (end_date) {  
            isInRange = (strcmp(date_str, end_date) <= 0); // 仅检查结束日期  
        }  
  
        if (isInRange && (keyword == NULL || strstr(buffer, keyword))) {  
            printf("%s", buffer);  
        }  
    }  
  
    fclose(file);  
}


int main(int argc, char *argv[]) {  

	if (argc < 2) { 
        printf("Usage: %s <path>\n", argv[0]);  
        return 1;  
    }  
  
    // 获取路径  
    char *path = argv[1];  
  
    // 使用GetFileAttributes获取文件属性  
    DWORD fileAttributes = GetFileAttributes(path);  
  
    // 检查函数是否成功执行  
    if (fileAttributes == 0) {  
        // 如果返回0，则调用失败，可能是因为路径不存在或无法访问  
        printf("Error accessing path '%s': Path not found or inaccessible.\n", path);  
        return 1;  
    }  
  
    // 检查路径是否指向一个文件夹  
    if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  
        printf("'%s' is a directory.\n", path);  

    WIN32_FIND_DATA FindFileData;  
    HANDLE hFind;  
    char searchPath[MAX_PATH];  
    const char* startDate = NULL;  
    const char* endDate = NULL;  
    const char* keyword = NULL;  
  
    // 检查命令行参数  
    if (argc < 2) {  
        printf("Usage: %s <directory> [start_date] [end_date] [keyword]\n", argv[0]);  
        return 1;  
    }  
    
    strcpy(searchPath, argv[1]);  
    strcat(searchPath, "\\*.log");  
  
    // 处理可选的日期范围和关键字参数  
    //if (argc >= 3) startDate = argv[2];  
    //if (argc >= 4) endDate = argv[3];  
    //if (argc >= 5) keyword = argv[4];  
	if (argc >= 3) keyword = argv[2];  
    if (argc >= 4) startDate = argv[3];  
    if (argc >= 5) endDate = argv[4];  
    hFind = FindFirstFile(searchPath, &FindFileData);  
    if (hFind == INVALID_HANDLE_VALUE) {  
        printf("Failed to find files.\n");  
        return 1;  
    }  
  
    do {  
        if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  
            char fullPath[MAX_PATH];  
            strcpy(fullPath, argv[1]);  
            strcat(fullPath, "\\");  
            strcat(fullPath, FindFileData.cFileName);  
  
            // 调用过滤函数处理文件  
            new_filter_by_date_range_and_keyword(fullPath, startDate, endDate, keyword);  
        }  
    } while (FindNextFile(hFind, &FindFileData) != 0); 
	//////////**************///////////////
    }  
	////////////////////////////////////否则为文件
    else
	{
		printf("'%s' is a file.\n", path);  
	 /******************///搜索
   if (argc < 3) {  
        printf("Usage: %s <logfile> <operation> [arguments...]\n", argv[0]);  
        printf("Operations:\n");  
        printf("  search <keyword>        - Search for lines containing <keyword>\n");  
        printf("  count <keyword>         - Count occurrences of <keyword>\n");  
  
        return 1;  
    }  
  
    const char* filename = argv[1];  
    const char* operation = argv[2];  
  
    // 无限循环 
    while (1) {  
        if (strcmp(operation, "search") == 0) {  
            search_keywords(filename, argv[3]); // 日志搜索函数  
        } else if (strcmp(operation, "count") == 0) {  
           	count_keywords(filename, argv[3]); // 关键字计数函数  
        } else if (strcmp(operation, "filter") == 0) {  
            new_filter_by_date_range_and_keyword(filename, argv[3], argv[4], argc >= 6 ? argv[5] : NULL);//日期范围和关键字过滤函数 
        }  
  
         
        printf("Waiting for the next hour...\n");  
        Sleep(3600000); // 一个小时（3600秒）   
		//Sleep(3600); // 测试
		//break;  // 便于测试
    }  

	}
  
    return 0;  //代码的完整性
}