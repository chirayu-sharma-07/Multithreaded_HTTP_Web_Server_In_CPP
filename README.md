While working with ESP32 Microcontroller, i encountered with one problem, since i am working on generic IoT Systems i need to configure devices like routers, sensors, IoT Hubs, IoT Gateways, Section Controllers, Sub-Section Controllers, etc.

For Configuration purpose i had one hand held device which has some buttons and LCD on it, i used to configure devices with it, but over time it became very difficult to configure things because pressing multiple buttons again and again, and carrying that device on installation sites is not easy.

Then i did R&D and found ESP32 has TCP/IP stack written in it, so one idea came in my mind why not write HTTP functionality in ESP32 so that it serves HTML, CSS, Javascript files to the clients like Mobile phone, Laptop, Desktop, Tablet, etc.

I already knew that ESP32 has WiFi functionality in it so devices can easily connect with it over wifi and ESP32 can easily server web pages.

So I am trying to write this HTTP Compliant web server which works with ESP32 and User uses it by making HTML, CSS, Javascript files. ESP32 will serve those files to connected devices. Connected devices with setup the configuration on web page will submit the response, ESP32 will receive the submit form, extracts the information and then configure other IoT devices like routers, sensors, IoT Hubs, etc via ESP_NOW Protocol (Since ESP_NOW is the base protocol written in ESP32 which works wirelessly so you can easily communicate with other ESP32 devices within the network).

In order to handle multiple clients at the same time, I will introduce multithreading later on so that one web server can handle multiple clients by assigning different thread to each connected client.

It will help me to configure multiple devices at the same time with my mobile phone without much hustle like before.

However I need to write the web application code at ESP32 side which will be using this web server.
