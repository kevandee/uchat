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

static char *swift_weather_namer(char *full) {
  char *new_name = NULL;

  if (mx_strstr(full, "інлив")) {
    new_name = mx_strdup("overcast");
    if (mx_strstr(full, "дощ")) {
      new_name = mx_strrejoin(new_name, "rain");
    }
  }
  else if (mx_strstr(full, "марн")) {
    new_name = mx_strdup("cloudy");
    if (mx_strstr(full, "дощ")) {
      new_name = mx_strrejoin(new_name, "rain");
    }
    else if (mx_strstr(full, "сніг")) {
      new_name = mx_strrejoin(new_name, "snow");
    }
  }
  else if (mx_strstr(full, "Туман")) {
    new_name = mx_strdup("fog");
  }
  else if (mx_strstr(full, "Ясно")) {
    new_name = mx_strdup("clear");
  }
  else {
    new_name = mx_strdup("unknown");
  }

  return new_name;
}

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

  char **label_array = malloc(5 * sizeof (char *));
  for (int i = 0; i < 5; i++) {
    label_array[i] = NULL;
  }

  label_array[0] = mx_strdup(weather_array[2]);// date

  label_array[1] = mx_strdup(weather_array[6]); // month

  label_array[2] = mx_strjoin(city, ", Україна");// city, Ukraine

  label_array[3] = mx_strjoin(weather_array[5], "°С"); // temperature 

  char *new_name = swift_weather_namer(weather_array[3]);
  char *ntemp = mx_strjoin("client/media/weather/", new_name);
  ntemp = mx_strrejoin(ntemp, ".png");
  label_array[4] = mx_strdup(ntemp);
  mx_strdel(&new_name);
  mx_strdel(&ntemp);

  mx_del_strarr(&weather_array);
  return label_array;
}
