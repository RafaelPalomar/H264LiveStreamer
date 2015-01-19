# H264LiveStreamer
Webcam H264 video encoding and streaming through RTP.

This source code is based on the Q&A found at:
<ul>
<li>http://stackoverflow.com/questions/19427576/live555-x264-stream-live-source-based-on-testondemandrtspserver
<li>http://stackoverflow.com/questions/13863673/how-to-write-a-live555-framedsource-to-allow-me-to-stream-h-264-live
</ul>

All the credit to the people involved in those questions and answers, especially
@bhawesh (http://stackoverflow.com/users/2024789/bhawesh) who provided
most of the code in which this repository is based on.

My contribution has been articulating the code in a project using CMake as well as
some small changes to make it work on GNU/Linux (Debian Jessie).

In order to build the project the following dependences are required:
<ul>
<li> OpenCV
<li> LiveMedia
<li> groupsock
<li> UsageEnvironment
<li> BasicUsageEnvironment
<li> x264
<li> tbb
<li> swscale
</ul>

