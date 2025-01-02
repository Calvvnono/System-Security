/* vulp.c */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
    char * fn = "/tmp/XYZ";
    char buffer[60];
    FILE *fp;
    uid_t ruid = getuid(); // 获取真实用户 ID
    uid_t euid = geteuid(); // 获取有效用户 ID
    seteuid(ruid); // 放弃 root 权限

    /* get user input */
    scanf("%50s", buffer );

    if(!access(fn, W_OK)){
        seteuid(euid); // 恢复 root 权限, 以便进行文件操作
        fp = fopen(fn, "a+");
        fwrite("\n", sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        fclose(fp);
    }
    else {
        printf("No permission \n");
    }

    seteuid(euid); // 恢复 root 权限
    return 0;
}
