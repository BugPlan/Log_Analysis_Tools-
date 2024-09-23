#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#include <windows.h>
#include <sys/stat.h>  
#include <direct.h>
#define MAX_PATH 260 


//������ǰ�ļ��к���
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
            // �ݹ����Ŀ¼  
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

// ���ڽ�������   ������ʽΪ YYYY-MM-DD  
int parse_date(const char* date_str, int* year, int* month, int* day) {  
    if (sscanf(date_str, "%d-%d-%d", year, month, day) != 3) {  
        return 0; // ����ʧ��  
    }  
    return 1; // �����ɹ�  
} 
  
// ���ڷ�Χ�ȽϺ���
int is_date_in_range(const char* date_str, const char* start_date, const char* end_date) {  
    int year, month, day;  
    int start_year, start_month, start_day;  
    int end_year, end_month, end_day;  
  
    if (!parse_date(date_str, &year, &month, &day) ||  
        !parse_date(start_date, &start_year, &start_month, &start_day) ||  
        !parse_date(end_date, &end_year, &end_month, &end_day)) {  
        return 0; // ����ʧ��  
    }  
  
    // ����Ƿ������ڷ�Χ��  
    if (year > end_year || (year == end_year && month > end_month) ||   
        (year == end_year && month == end_month && day > end_day)) {
        return 0; // ���ڳ�����Χ  
    }  
    if (year < start_year || (year == start_year && month < start_month) ||   
        (year == start_year && month == start_month && day < start_day)) {  
        return 0; // �����ڷ�Χ֮ǰ  
    }  
  
    return 1; // �����ڷ�Χ��  
}
  
// �򻯵����ڴ�����ʱ����true���������ڼ��  
/*int is_date_in_range(const char* date_str, const char* start_date, const char* end_date) {  
    return 1; // ���Ƿ���true�����������ڼ��  
}*/

// ��־��������   
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
  
// �ؼ��ּ�������  
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

//���ڷ�Χ�͹ؼ��ֹ��˺���
void filter_by_date_range_and_keyword(const char* filename, const char* start_date, const char* end_date, const char* keyword) {  
    FILE *file = fopen(filename, "r");  
    if (!file) {  
        perror("Failed to open file");  
        return;  
    }  
  
    char buffer[1024];  
    while (fgets(buffer, sizeof(buffer), file)) {  
        // ��־�ĵ�һ����������  
        char date_str[20];  
        sscanf(buffer, "%19[^ ]", date_str); // ��ȡֱ���ո���ַ��� 
  
        if (is_date_in_range(date_str, start_date, end_date)) {  
            if (keyword == NULL || strstr(buffer, keyword)) {  
                printf("%s", buffer);  
            }  
        }  
    }  
  
    fclose(file);  
}    
///!!!!�Ľ����ڷ�Χ�͹ؼ��ֹ��˺���
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
  
        int isInRange = 1; // Ĭ���ڷ�Χ��  
        if (start_date && end_date) {  
            isInRange = is_date_in_range(date_str, start_date, end_date);  
        } else if (start_date) {  
            isInRange = (strcmp(date_str, start_date) >= 0); // ����鿪ʼ����  
        } else if (end_date) {  
            isInRange = (strcmp(date_str, end_date) <= 0); // ������������  
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
  
    // ��ȡ·��  
    char *path = argv[1];  
  
    // ʹ��GetFileAttributes��ȡ�ļ�����  
    DWORD fileAttributes = GetFileAttributes(path);  
  
    // ��麯���Ƿ�ɹ�ִ��  
    if (fileAttributes == 0) {  
        // �������0�������ʧ�ܣ���������Ϊ·�������ڻ��޷�����  
        printf("Error accessing path '%s': Path not found or inaccessible.\n", path);  
        return 1;  
    }  
  
    // ���·���Ƿ�ָ��һ���ļ���  
    if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  
        printf("'%s' is a directory.\n", path);  

    WIN32_FIND_DATA FindFileData;  
    HANDLE hFind;  
    char searchPath[MAX_PATH];  
    const char* startDate = NULL;  
    const char* endDate = NULL;  
    const char* keyword = NULL;  
  
    // ��������в���  
    if (argc < 2) {  
        printf("Usage: %s <directory> [start_date] [end_date] [keyword]\n", argv[0]);  
        return 1;  
    }  
    
    strcpy(searchPath, argv[1]);  
    strcat(searchPath, "\\*.log");  
  
    // �����ѡ�����ڷ�Χ�͹ؼ��ֲ���  
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
  
            // ���ù��˺��������ļ�  
            new_filter_by_date_range_and_keyword(fullPath, startDate, endDate, keyword);  
        }  
    } while (FindNextFile(hFind, &FindFileData) != 0); 
	//////////**************///////////////
    }  
	////////////////////////////////////����Ϊ�ļ�
    else
	{
		printf("'%s' is a file.\n", path);  
	 /******************///����
   if (argc < 3) {  
        printf("Usage: %s <logfile> <operation> [arguments...]\n", argv[0]);  
        printf("Operations:\n");  
        printf("  search <keyword>        - Search for lines containing <keyword>\n");  
        printf("  count <keyword>         - Count occurrences of <keyword>\n");  
  
        return 1;  
    }  
  
    const char* filename = argv[1];  
    const char* operation = argv[2];  
  
    // ����ѭ�� 
    while (1) {  
        if (strcmp(operation, "search") == 0) {  
            search_keywords(filename, argv[3]); // ��־��������  
        } else if (strcmp(operation, "count") == 0) {  
           	count_keywords(filename, argv[3]); // �ؼ��ּ�������  
        } else if (strcmp(operation, "filter") == 0) {  
            new_filter_by_date_range_and_keyword(filename, argv[3], argv[4], argc >= 6 ? argv[5] : NULL);//���ڷ�Χ�͹ؼ��ֹ��˺��� 
        }  
  
         
        printf("Waiting for the next hour...\n");  
        Sleep(3600000); // һ��Сʱ��3600�룩   
		//Sleep(3600); // ����
		//break;  // ���ڲ���
    }  

	}
  
    return 0;  //�����������
}