#include "../inc/ush.h"

int ush_parser(char *src, t_ush *ush) {
    char *cmd_table[] = {"export", "unset", "fg","env", 
                         "cd", "pwd", "which", "echo", 
                         "exit"};

    char *tr_src = mx_strtrim(src);
    //printf("%s", tr_src);
    //printf("\n");
    char **src_arr = ush_strsplit(tr_src, ush);
    ush->argv_arr = ush_strsplit(tr_src, ush);
    mx_strdel(&tr_src);
    if (!src_arr) {
        return 1;
    }
    bool check = false;
    for (int i = 0; i < 9; i++) {
        if (mx_strcmp(cmd_table[i], src_arr[0]) == 0) {
            check = true;
            ush->cmd_num = i;
        }
    }
    if (!check) {
        ush->cmd_num = -1;
        //return 127;
    }
    
    for (int i = 1; src_arr[i] != NULL; i++) {
        bool flag = false;
        int flag_d = 1;
        bool dnt = false;

        for (int j = 0; j < mx_strlen(src_arr[i]); j++) {
            if (src_arr[i][j] == '$') {
                bool skobka = src_arr[i][j + 1] == '(' ? true : false;
                //fprintf(stderr, "src %s\n", src_arr[i]);
                int count_sk = 1;
                int k = j + 1;
                //int start = k;
                int len = 0;
                if(src_arr[i][j + 1] == '?' && ush -> cmd_num == 7){
                    //mx_printint(ush->err);
                    //mx_printchar('\n');
                    char *return_str = mx_itoa(ush->err);
                    char *temp1 = mx_replace_substr(src_arr[i], "$?", return_str);
                    mx_strdel(&src_arr[i]);
                    src_arr[i] = temp1;
                    mx_strdel(&return_str);
                    break;
                }
 
                if (!skobka){
                    while (/*src_arr[i][k] != ' ' &&*/ src_arr[i][k] != '\'' && src_arr[i][k] != '\"' && src_arr[i][k] != '\0') {
                        len++;
                        k++;
                        if(src_arr[i][k] == '\''){
                            flag_d *= -1;
                        }
                    }
                }
                else {
                    while (src_arr[i][k] != '\0' && count_sk > 0) {
                        if (src_arr[i][k] == '(') {
                            count_sk++;
                        }
                        if (src_arr[i][k] == ')') {
                            count_sk--;
                        }
                        k++;
                        len++;
                    }
                    if (src_arr[i][k] == ')' && src_arr[i][k - 1] == ')') {
                        len--;
                    }
                }

                if(flag_d == 1){
                    /*if (src_arr[i][j + 1 + len] == ')' && src_arr[i][j + len] == ')') {
                        len--;
                    }*/
                    char *env_var = mx_strndup(src_arr[i] + j + 1, len);
                    //fprintf(stderr, "env var %s\n", env_var);
                    char *temp = mx_strjoin("$", env_var);

                    if(env_var[0] == '{' && env_var[mx_strlen(env_var) - 1] == '}'){
                        char *temp_temp = mx_strdup(env_var);
                        int size = mx_strlen(temp_temp) - 2;
                        char *res_s = mx_strnew(size);
                        int k = 0;

                        for(int i = 0; i < mx_strlen(temp_temp); i++){
                            if(temp_temp[i] == '{' || temp_temp[i] == '}'){

                                continue;
                            }
                            else{
                                res_s[k] = temp_temp[i];
                                k++;
                            }
                        }

                        char *res = getenv(res_s);

                        mx_strdel(&res_s);
                        mx_strdel(&env_var);
                        if (res) {
                            mx_push_back(&(ush->argv), mx_replace_substr(src_arr[i], temp, res)); 
                            flag = true;
                        }
                        else{
                            dnt = true;
                        }
                    }
                    if(env_var[0] == '(' && env_var[mx_strlen(env_var) - 1] == ')'){
                        char *temp_temp = mx_strdup(env_var);
                        j += len;
                        int size = mx_strlen(temp_temp) - 2;
                        char *res_s = mx_strnew(size);
                        int k = 0;

                        for(int i = 1; i < mx_strlen(temp_temp) - 1; i++){
                            /*if(temp_temp[i] == '(' || temp_temp[i] == ')'){
                                continue;
                            }*/
                            //else
                            if (temp_temp[i] != ')')
                            {
                                res_s[k] = temp_temp[i];
                                k++;
                            }
                        }
                        //printf("temp_temp %s\n", temp_temp);
                        //printf("res %s\n", res_s);

                        char *file = "redir";


                        /*fflush(stdout);
                        int fd1 = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        int fd2 = dup(STDOUT_FILENO);
                        dup2(fd1, STDOUT_FILENO);
                        close(fd1);*/
                        t_ush *temp_ush;
                        init(&temp_ush);
                        fpos_t pos;
                        int fd;
                        //if (!ush->redirect) {
                        fgetpos(stdout, &pos);  // save the position in the file stream
                        fd = dup(fileno(stdout));  // use the dup() function to create a copy of stdout
        
                        freopen(file, "w", stdout);  // redirect stdout
                        //fclose(stdout);
                        //}
                        FILE *file_p = fopen(file, "w");
                        fclose(file_p);
                        int cmd_num = ush->cmd_num;
                        ush->redirect++;
                        temp_ush->cmd_num = ush->cmd_num;
                        char *var = mx_strndup(env_var + 1, mx_strlen(env_var) - 2);
                        ush_parser(var, temp_ush);

                        ush_execute(temp_ush);

                        ush->cmd_num = cmd_num;
                        ush->redirect--;
                        //if (!ush->redirect) {
                        fflush(stdout);
 
                        dup2(fd, fileno(stdout));  // restore the stdout

                        close(fd);
                        clearerr(stdout);  

                        fsetpos(stdout, &pos); // move to the correct position
                        //}
                        //fopen(stdout);
                        //fflush(stdout);
                        //dup2(fd2, STDOUT_FILENO);
                            //return err_report("Failed to reinstate standard output\n");
                        //close(fd2);
                        //mx_strdel(&res_s);
                        
                        mx_strdel(&env_var);
                        char *repl = mx_file_to_str(file);
                        char *repl_tr = mx_del_extra_spaces(repl);
                        mx_strdel(&repl);
                        //fprintf(stderr, "3\n");
                        if (res_s) {
                            mx_push_back(&(ush->argv), mx_replace_substr(src_arr[i], temp, repl_tr)); 
                            //fprintf(stderr, "\n%d   %s | %s | %s\n", ush->redirect, src_arr[i],  temp, mx_replace_substr(src_arr[i], temp, repl_tr));
                            
                            flag = true;
                        }
                        else {
                            dnt = true;
                        }
                    }
                    else{
                        char *res = getenv(env_var);
                        mx_strdel(&env_var);
                        if (res) {
                            mx_push_back(&(ush->argv), mx_replace_substr(src_arr[i], temp, res)); 
                            flag = true;
                        }
                    }
                }
            }         
        }
        if (!flag && !dnt)
            mx_push_back(&(ush->argv), mx_strdup(src_arr[i]));
    }
    
    mx_strdel(&tr_src);
    mx_del_strarr(&src_arr);
    return 0;
}
