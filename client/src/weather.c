#include "../inc/uch_client.h"

static size_t write_data1(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

static char *get_weather(char *city) {
    FILE *fp = fopen("weather_client.txt","wb");////// ???????
    CURL *curl;
    (void)city;
  CURLcode res;
    
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://ua.sinoptik.ua/погода-харків");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.47.1");
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data1); 
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
  return mx_file_to_str("weather_client.txt");
}

static char *swift_weather_trim(char *string, char final_char) {
  int counter = 0;
  while (string[counter] != final_char) {
    counter ++;
  }

  return mx_strndup(string, counter);
}

char **weather_parse(char *city) {
  char *weather_string = get_weather(city);

  char **weather_array = malloc(8 * sizeof (char *));
  for (int i = 0; i < 8; i++) {
    weather_array[i] = NULL;
  }

  /*char *temp = mx_strstr(weather_string, "k=\"//ua.sinoptik.ua/погода-");
  temp += mx_strlen("k=\"//ua.sinoptik.ua/погода-");
  temp = mx_strchr(temp, '/') + 1;
  weather_array[0] = swift_weather_trim(temp, '\"');  // Date (Format: 2022-02-06)
  printf("\n%s\n", weather_array[0]);*/

  char *temp = mx_strstr(weather_string, "k=\"//ua.sinoptik.ua/погода-");
  temp += mx_strlen("k=\"//ua.sinoptik.ua/погода-");
  temp = mx_strchr(temp, '/') + 1;
  weather_array[0] = swift_weather_trim(temp, '-');  // Year (Format: 2022)
  
  temp = mx_strchr(temp, '-') + 1;
  weather_array[1] = swift_weather_trim(temp, '-'); // Month (Format: 02)

  temp = mx_strchr(temp, '-') + 1;
  weather_array[2] = swift_weather_trim(temp, '\"'); // Day (Format: 06)

  temp = mx_strstr(temp, "<div class=\"weatherIc");
  temp += mx_strlen("<div class=\"weatherIc");
  temp = mx_strchr(temp, '=');
  temp +=2;                                           // Weather conditions 
  weather_array[3] = swift_weather_trim(temp, '\"');  //(Format: Хмарно з проясненнями, слабкий сніг)

  temp = mx_strstr(temp, "<span>");  //SPANKING!!!!
  temp += mx_strlen("<span>");
  weather_array[4] = swift_weather_trim(temp, '&'); // min temp (Format: -4)

  temp = mx_strstr(temp, "<span>");  //SPANKING!!!!
  temp += mx_strlen("<span>");
  weather_array[5] = swift_weather_trim(temp, '&'); // max temp (Format: 1)

  temp = mx_strstr(temp, "<p class=\"month\"");
  temp += mx_strlen("<p class=\"month\"") + 1;
  weather_array[6] = swift_weather_trim(temp, '<'); // Month (Format:лютого)

  /*char *temp = mx_strstr(weather_string, "<p class=\"day-link\"");
  temp = mx_strchr(temp, '>') + 1;
  weather_array[0] = swift_weather_trim(temp, '<');   //Day of week (Format:Неділя) 
  printf("\n%s\n", weather_array[0]);*/

  char **label_array = malloc(3 * sizeof (char *));
  for (int i = 0; i < 3; i++) {
    label_array[i] = NULL;
  }

  char *ntemp = mx_strjoin(city, ": ");
  ntemp = mx_strrejoin(ntemp, weather_array[5]);
  ntemp = mx_strrejoin(ntemp, " ");
  ntemp = mx_strrejoin(ntemp, weather_array[4]);
  label_array[0] = mx_strdup(ntemp);
  mx_strdel(&ntemp);

  ntemp = mx_strjoin("client/media/weather/", weather_array[3]);
  ntemp = mx_strrejoin(ntemp, ".jpg");
  printf("\n%s\n%s\n", weather_array[3], ntemp);
  label_array[1] = mx_strdup(ntemp);
  mx_strdel(&ntemp);

  ntemp = mx_strjoin(weather_array[2], " ");
  ntemp = mx_strrejoin(ntemp, weather_array[6]);
  ntemp = mx_strrejoin(ntemp, " ");
  ntemp = mx_strrejoin(ntemp, weather_array[0]);
  label_array[2] = mx_strdup(ntemp);
  mx_strdel(&ntemp);

  mx_del_strarr(&weather_array);
  return label_array;
}

/*char **weather_parse(char *city) {
  char *weather_string = get_weather(city);
  printf("\n%s\n", weather_string);
  char **weather_array = malloc(3 * sizeof (char *));
  for (int i = 0; i < 3; i++) {
    weather_array[i] = NULL;
  }

  char *temp = mx_strchr(weather_string, '\n');
  temp += 33;
  weather_array[0] = mx_strndup(temp, mx_get_char_index(temp, '\n')); // weather type (cloudy, sunny)

  temp = mx_strchr(temp, '\n');
  temp += 44;
  weather_array[1] = swift_weather_trim(temp); //temperature day

  temp += mx_strlen(weather_array[1]) + 16;
  weather_array[2] = swift_weather_trim(temp);  //temperature night


  char **label_out_array = malloc(3 * sizeof (char *));
  for (int i = 0; i < 3; i++) {
    label_out_array[i] = NULL;
  }

  temp = mx_strjoin(city, "\n");
  temp = mx_strrejoin(temp, weather_array[1]);
  temp = mx_strrejoin(temp, "(");
  temp = mx_strrejoin(temp, weather_array[2]);
  temp = mx_strrejoin(temp, ")°C");
  label_out_array[0] = mx_strdup(temp);// 'format:' Kharkov \n -5°C
  printf("\n%s\n", temp);
  mx_strdel(&temp);

  temp = mx_strdup("Date\nhere");
  label_out_array[1] = mx_strdup(temp);// 'format:' December \n  31 \n 2020
  printf("\n%s\n", temp);
  mx_strdel(&temp);

  temp = mx_strjoin("client/media/weather/weather-", weather_array[0]);
  temp = mx_strrejoin(temp, ".jpg");
  label_out_array[2] = mx_strdup(temp); // weather type (cloudy, sunny)
  printf("\n%s\n", temp);
  mx_strdel(&temp);

  mx_del_strarr(&weather_array);
  mx_strdel(&weather_string);
  return label_out_array;
}*/
