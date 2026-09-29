#pragma once
#include <Iroha.h>

static size_t WriteCallback(void* ptr, size_t size, size_t nmemb, std::string* data)
{
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}
struct RealWeatherData
{
	std::string area;
	std::string todayWeather;
	std::string rainChance;
};

class RealWeatherManager :public Singleton<RealWeatherManager>
{
private:

	RealWeatherData realWeatherData;
public:
	bool Update()
	{
		std::string url =
			"https://www.jma.go.jp/bosai/forecast/data/forecast/270000.json";

		std::string response;

		CURL* curl = curl_easy_init();
		if (!curl) return false;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK)
			return false;

		Parse(response);
		return true;
	}

	bool isCheckOnline()
	{
		CURL* curl = curl_easy_init();
		if (!curl) return false;
		else return true;
	}

	RealWeatherData& GetWeatherData()
	{
		return realWeatherData;
	}

	/*
	void Print() const
	{
		std::cout << "地域: " << area << "\n";
		std::cout << "今日の天気: " << todayWeather << "\n";
		std::cout << "降水確率: " << rainChance << "%\n";
	}
	*/

private:

	void Parse(const std::string& text)
	{
		json j = json::parse(text);

		realWeatherData.area = j[0]["timeSeries"][0]["areas"][0]["area"]["name"];
		realWeatherData.todayWeather = j[0]["timeSeries"][0]["areas"][0]["weathers"][0];

		realWeatherData.rainChance =
			j[0]["timeSeries"][1]["areas"][0]["pops"][0];
	}
};
