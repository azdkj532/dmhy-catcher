dmhy-catcher
============
autometically catch dmhy magnet

At first, we get keywords from mysql server, and then use wget(on linux) to get the web page contains the searching result. Then, analyze it to get the title of each topic. To avoid utf-8 encoding problem, I hashed the title. So we can search hashing value of title to ensure that the source hasn't been add yet. After we get the link of topic, we use wget again to get the topic, finding the magnet link. At last, we send the control message encoded in json to transmission to start the downloading task.

P.S. All tasks are sorted by task name.

require packages
libcurl3
libmysql-client18
