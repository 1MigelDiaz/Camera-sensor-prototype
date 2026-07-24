#include <opencv2/opencv.hpp>
#include <chrono>
#include <iostream>
#include <SFML/Audio.hpp>

int main() {
	cv::VideoCapture camera(0);
	cv::Mat frame;
	cv::Mat reference;
	camera >> reference;
	cv::Mat difference;
	cv::Mat gray;
	cv::Mat thresholdframe;
	int framecount = 0;
	bool played = false;

	bool detected = false;
	auto lastAlarm = std::chrono::steady_clock::now();
	sf::SoundBuffer alarmsound;
	alarmsound.loadFromFile("/Programming_assets/beep2.mp3");
	sf::Sound alarmbeep(alarmsound);

	while (true)
	{
		camera >> frame;
		cv::absdiff(reference, frame, difference);
		cv::cvtColor(difference, gray, cv::COLOR_BGR2GRAY);

		cv::threshold(gray, thresholdframe, 30, 255, cv::THRESH_BINARY);

		int change = cv::countNonZero(thresholdframe);

		if (change >= 5000) // sensitivity
		{
			detected = true; 
		}
		else { detected = false; }

		if (framecount < 500)
		{
			framecount += 1;
		}
		else if (framecount >= 500)
		{
			framecount = 0;
		}

		if (detected == true)
		{
			auto currentTime = std::chrono::steady_clock::now();
			auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastAlarm);

			if (seconds.count() >= 1.2)
			{
				lastAlarm = currentTime;
				std::cout << framecount << ":Human Detected!\n";
			}
			if (played == false)
			{
				alarmbeep.play(); played = true;
			}
		}
		else { played = false; }
		auto currentTime = std::chrono::steady_clock::now();
		auto newsec = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastAlarm);
		if (detected == false && newsec.count() >= 3)
		{
			std::cout << framecount << ":Clear\n";
		}

		cv::imshow("Camera", frame);
		cv::imshow("Motion", thresholdframe);

		if (cv::waitKey(1) == 27) { break; }
	}
	camera.release();
	cv::destroyAllWindows();
}