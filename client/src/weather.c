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
    //curl_easy_setopt(curl, CURLOPT_URL, "https://www.accuweather.com/en/ua/kharkiv/323903/weather-forecast/323903");
    //curl_easy_setopt(curl, CURLOPT_URL, "https://www.timeanddate.com/weather/ukraine/kharkiv");
    curl_easy_setopt(curl, CURLOPT_URL, "https://ua.sinoptik.ua/погода-харків");
    //curl_easy_setopt(curl, CURLOPT_URL, "https://en.tutiempo.net/kharkiv.html");
    //curl_easy_setopt(curl, CURLOPT_URL, "https://m.rp5.ua/Weather_in_Kharkiv");
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

/*char **weather_parse(char *city) {
  char *weather_string = get_weather(city);
  //printf("\n\n%s\n", weather_string);

  char **weather_array = malloc(8 * sizeof (char *));
  for (int i = 0; i < 8; i++) {
    weather_array[i] = NULL;
  }

  char *temp = mx_strstr(weather_string, "Kharkiv");
  weather_array[0] = swift_weather_trim(temp, '>');

  return weather_array;
}*/

char **weather_parse(char *city) {
  char *weather_string = get_weather(city);

  char **weather_array = malloc(8 * sizeof (char *));
  for (int i = 0; i < 8; i++) {
    weather_array[i] = NULL;
  }

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
