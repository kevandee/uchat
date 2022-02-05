#include "../inc/ush.h"

static void escape_ch(char *temp, int i, char *res, int k){
    if (temp[i] == 'a'){
        res[k] = '\a';
    }
    else if(temp[i] == 'n'){
        res[k] = '\n';
    }
    else if(temp[i] == 'r'){
        res[k] = '\r';
    }
    else if(temp[i] == 't'){
        res[k] = '\t';
    }
    else if(temp[i] == 'b'){
        res[k] = '\b';
    }
    else if(temp[i] == 'f'){
        res[k] = '\f';
    }
    else if(temp[i] == 'v'){
        res[k] = '\v';
    }
}

char *del_quot_echo(char *src, bool flag_E) {
    int flag_single = 1;
    int flag_double = 1;
    int counter_quotes = 0;

    char *temp = mx_strdup(src);
    int count_quot = 0;
    for(int i = 0; i < mx_strlen(temp); i++) {
        if (temp[i] == '\'' ||temp[i] == '\"') {
            count_quot++;
        }
    }
    
    int size = mx_strlen(temp) - count_quot;
    char *res = mx_strnew(size);
    int k = 0;

    for (int i = 0; i < mx_strlen(temp); i++) {
        if(flag_E == false){
            if(temp[i] == '\'' || temp[i] == '\"'){
                if(temp[i] == '\''){
                    flag_single *= -1;
                }
                else{
                    flag_double *= -1;
                }
            }
            else if(temp[i] == '\\' && flag_single == -1){
                counter_quotes++;
                if(counter_quotes % 2 == 1){
                    i++;
                    if(temp[i] == 'a'){
                        res[k] = '\a';
                    }
                    else if(temp[i] == 'n'){
                        res[k] = '\n';
                    }
                    else if(temp[i] == 'r'){
                        res[k] = '\r';
                    }
                    else if(temp[i] == 't'){
                        res[k] = '\t';
                    }
                    else if(temp[i] == 'b'){
                        res[k] = '\b';
                    }
                    else if(temp[i] == 'f'){
                        res[k] = '\f';
                    }
                    else if(temp[i] == 'v'){
                        res[k] = '\v';
                    }
                    else{
                        res[k] = temp[i];
                        i--;
                    }
                    k++;
                }
            }
            else if(temp[i] == '\\' && flag_double == -1){
                if(temp[i + 1] != '\\'){
                    i++;
                    escape_ch(temp, i, res, k);
                    k++;
                }
                else if(temp[i + 2] != '\\'){
                    i += 2;
                    escape_ch(temp, i, res, k);
                    k++;
                }
                else if(temp[i + 3] != '\\'){
                    i += 3;
                    res[k] = '\\';
                    k++;
                    res[k] = temp[i];
                    k++;
                }
                else if(temp[i + 4] != '\\'){
                    i += 4;
                    res[k] = '\\';
                    k++;
                    res[k] = temp[i];
                    k++;
                }
                else if(temp[i + 5] != '\\'){
                    i += 5;
                    res[k] = '\\';
                    k++;
                    escape_ch(temp, i, res, k);
                    k++;
                }
                else if(temp[i + 6] != '\\'){
                    i += 6;
                    res[k] = '\\';
                    k++;
                    escape_ch(temp, i, res, k);
                    k++;
                }
            }
            else if(temp[i] == '\\' && flag_single == 1 && flag_double == 1){
                if(temp[i + 1] != '\\'){
                    i++;
                    res[k] = temp[i];
                    k++;
                }
                else if(temp[i + 2] != '\\'){
                    i += 2;
                    escape_ch(temp, i, res, k);
                    k++;
                }
                else if(temp[i + 3] != '\\'){
                    i += 3;
                    escape_ch(temp, i, res, k);
                    k++;
                }
                else if(temp[i + 4] != '\\'){
                    i += 4;
                    res[k] = '\\';
                    k++;
                    res[k] = temp[i];
                    k++;
                }
                else if(temp[i + 5] != '\\'){
                    i += 5;
                    res[k] = '\\';
                    k++;
                    res[k] = temp[i];
                    k++;
                }
                else if(temp[i + 6] != '\\'){
                    i += 6;
                    res[k] = '\\';
                    k++;
                    escape_ch(temp, i, res, k);
                    k++;
                }
            }
            else{
                res[k] = temp[i];
                k++;
            }
        }
        else{
            //if()
            res[k] = temp[i];
            k++;
        }
    }
    res[k] = '\0';
    return res;
}

void ush_echo(t_ush *ush){
    bool flag_n = false;
    bool flag_e = false;
    bool flag_E = false;
    //fprintf (stderr, "echo %s\n", ush->argv->data);

    if (!ush->argv) {
        mx_printchar('\n');
        return;
    }

    char **arr = NULL;
    t_list *f_list = NULL;

    if(mx_strncmp(ush->argv->data, "-*", 1) == 0){
        if(mx_strcmp(ush->argv->data, "-n") == 0){
            flag_n = true;
        }
        else if(mx_strcmp(ush->argv->data, "-e") == 0){
            flag_e = true;
        }
        else if(mx_strcmp(ush->argv->data, "-E") == 0){
            flag_E = true;
        }
        else if(mx_strcmp(ush->argv->data, "-En") == 0|| mx_strcmp(ush->argv->data, "-nE") == 0){
            flag_E = true;
            flag_n = true;
        }
        ush->argv = ush->argv->next;
    }

    list_to_arr(ush->argv, &arr);

    for(int i = 0; i < mx_list_size(ush->argv); i++){
        if(arr[i]){
            mx_push_back(&f_list, del_quot_echo(arr[i], flag_E));
        }
    }

    /*while(ush->argv){
        mx_printstr(ush->argv->data);
        if(ush->argv->data != NULL){
            mx_printchar(' ');
        }
        ush->argv = ush->argv->next;
    }*/
    if(!f_list){
        return;
    }

    while(f_list){
        mx_printstr(f_list->data);
        if(f_list->next != NULL){
            mx_printchar(' ');
        }
        f_list = f_list->next;
    }
    if(!flag_n){
        mx_printchar('\n');
    }
    ush->err = 0;
}
