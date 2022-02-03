#include "../inc/uch_server.h"

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

char *get_facts() {
    FILE *fp = fopen("facts.txt","wb");
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.beagreatteacher.com/daily-fun-fact/");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.47.1");
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

    }
    fclose(fp);
    char *final = NULL;
    char *str = mx_file_to_str("facts.txt");
    str = mx_strstr(str, "Thought of the Day:");
    str = mx_strchr(str, ';')+1;
    int len = 0;
    while (*(str + len) != '&') {
        len++;
    }
    char *temp = mx_strndup(str, len);
    final = mx_strrejoin(final, "Thought of the Day: \"");
    final = mx_strrejoin(final, temp);
    final = mx_strrejoin(final, "\" - ");

    str = mx_strstr(str, "&#8221;&#8211;") +14;
    len = 0;
    while (*(str + len) != '<') {
        len++;
    }

    temp = mx_strndup(str, len);
    final = mx_strrejoin(final, temp);

    str = mx_strstr(str, "Joke of the Day:") + 21 + 16;
    len = 0;
    while (*(str + len) != '<') {
        len++;
    }
    temp = mx_strndup(str, len);
    final = mx_strrejoin(final, "\nJoke of the Day: ");
    final = mx_strrejoin(final, temp);

    str = mx_strstr(str, "Random Fact of the Day:") + 21 + 24;
    len = 0;
    while (*(str + len) != '<') {
        len++;
    }
    temp = mx_strndup(str, len);
    final = mx_strrejoin(final, "\nRandom Fact of the Day: ");
    final = mx_strrejoin(final, temp);

    str = mx_strstr(str, "Journal Entry Idea:") + 21 + 20;
    len = 0;
    while (*(str + len) != '<') {
        len++;
    }
    temp = mx_strndup(str, len);
    final = mx_strrejoin(final, "\nJournal Entry Idea: ");
    final = mx_strrejoin(final, temp);
    return final;
}
