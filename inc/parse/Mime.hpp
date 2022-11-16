#ifndef MIME_HPP
#define MIME_HPP

#include <map>
#include <string>

class Mime : public std::map<std::string, std::string>
{
private:
	Mime(const Mime&);
//	Mime& operator=(const Mime&);
public:
	Mime();
	~Mime();
};
Mime::~Mime(){};
Mime::Mime()
{
	(*this)[".html"] = "text/html";
	(*this)[".htm"] = "text/html";
	(*this)[".shtml"] = "text/html";
	(*this)[".css"] = "text/css";
	(*this)[".xml"] = "text/xml";
	(*this)[".gif"] = "image/gif";
	(*this)[".jpeg"] = "image/jpeg";
	(*this)[".jpg"] = "image/jpeg";
	(*this)[".js"] = "application/javascript";
	(*this)[".atom"] = "application/atom+xml";
	(*this)[".rss"] = "application/rss+xml";
	(*this)[".mml"] = "text/mathml";
	(*this)[".txt"] = "text/plain";
	(*this)[".jad"] = "text/vnd.sun.j2me.app-descriptor";
	(*this)[".wml"] = "text/vnd.wap.wml";
	(*this)[".htc"] = "text/x-component";
	(*this)[".avif"] = "image/avif";
	(*this)[".png"] = "image/png";
	(*this)[".svg"] = "image/svg+xml";
	(*this)[".svgz"] = "image/svg+xml";
	(*this)[".tif"] = "image/tiff";
	(*this)[".tiff"] = "image/tiff";
	(*this)[".wbmp"] = "image/vnd.wap.wbmp";
	(*this)[".webp"] = "image/webp";
	(*this)[".ico"] = "image/x-icon";
	(*this)[".jng"] = "image/x-jng";
	(*this)[".bmp"] = "image/x-ms-bmp";
	(*this)[".woff"] = "font/woff";
	(*this)[".woff2"] = "font/woff2";
	(*this)[".jar"] = "application/java-archive";
	(*this)[".war"] = "application/java-archive";
	(*this)[".ear"] = "application/java-archive";
	(*this)[".json"] = "application/json";
	(*this)[".hqx"] = "application/mac-binhex40";
	(*this)[".doc"] = "application/msword";
	(*this)[".pdf"] = "application/pdf";
	(*this)[".ps"] = "application/postscript";
	(*this)[".eps"] = "application/postscript";
	(*this)[".ai"] = "application/postscript";
	(*this)[".rtf"] = "application/rtf";
	(*this)[".m3u8"] = "application/vnd.apple.mpegurl";
	(*this)[".kml"] = "application/vnd.google-earth.kml+xml";
	(*this)[".kmz"] = "application/vnd.google-earth.kmz";
	(*this)[".xls"] = "application/vnd.ms-excel";
	(*this)[".eot"] = "application/vnd.ms-fontobject";
	(*this)[".ppt"] = "application/vnd.ms-powerpoint";
	(*this)[".odg"] = "application/vnd.oasis.opendocument.graphics";
	(*this)[".odp"] = "application/vnd.oasis.opendocument.presentation";
	(*this)[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	(*this)[".odt"] = "application/vnd.oasis.opendocument.text";
	(*this)[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	(*this)[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	(*this)[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	(*this)[".wmlc"] = "application/vnd.wap.wmlc";
	(*this)[".wasm"] = "application/wasm";
	(*this)[".7z"] = "application/x-7z-compressed";
	(*this)[".cco"] = "application/x-cocoa";
	(*this)[".jardiff"] = "application/x-java-archive-diff";
	(*this)[".jnlp"] = "application/x-java-jnlp-file";
	(*this)[".run"] = "application/x-makeself";
	(*this)[".pl"] = "application/x-perl";
	(*this)[".pm"] = "application/x-perl";
	(*this)[".prc"] = "application/x-pilot";
	(*this)[".pdb"] = "application/x-pilot";
	(*this)[".rar"] = "application/x-rar-compressed";
	(*this)[".rpm"] = "application/x-redhat-package-manager";
	(*this)[".sea"] = "application/x-sea";
	(*this)[".swf"] = "application/x-shockwave-flash";
	(*this)[".sit"] = "application/x-stuffit";
	(*this)[".tcl"] = "application/x-tcl";
	(*this)[".tk"] = "application/x-tcl";
	(*this)[".der"] = "application/x-x509-ca-cert";
	(*this)[".pem"] = "application/x-x509-ca-cert";
	(*this)[".crt"] = "application/x-x509-ca-cert";
	(*this)[".xpi"] = "application/x-xpinstall";
	(*this)[".xhtml"] = "application/xhtml+xml";
	(*this)[".xspf"] = "application/xspf+xml";
	(*this)[".zip"] = "application/zip";
	(*this)[".bin"] = "application/octet-stream";
	(*this)[".exe"] = "application/octet-stream";
	(*this)[".dll"] = "application/octet-stream";
	(*this)[".deb"] = "application/octet-stream";
	(*this)[".dmg"] = "application/octet-stream";
	(*this)[".iso"] = "application/octet-stream";
	(*this)[".img"] = "application/octet-stream";
	(*this)[".msi"] = "application/octet-stream";
	(*this)[".msp"] = "application/octet-stream";
	(*this)[".msm"] = "application/octet-stream";
	(*this)[".mid"] = "audio/midi";
	(*this)[".midi"] = "audio/midi";
	(*this)[".kar"] = "audio/midi";
	(*this)[".mp3"] = "audio/mpeg";
	(*this)[".ogg"] = "audio/ogg";
	(*this)[".m4a"] = "audio/x-m4a";
	(*this)[".ra"] = "audio/x-realaudio";
	(*this)[".3gpp"] = "video/3gpp";
	(*this)[".3gp"] = "video/3gpp";
	(*this)[".ts"] = "video/mp2t";
	(*this)[".mp4"] = "video/mp4";
	(*this)[".mpeg"] = "video/mpeg";
	(*this)[".mpg"] = "video/mpeg";
	(*this)[".mov"] = "video/quicktime";
	(*this)[".webm"] = "video/webm";
	(*this)[".flv"] = "video/x-flv";
	(*this)[".m4v"] = "video/x-m4v";
	(*this)[".mng"] = "video/x-mng";
	(*this)[".asx"] = "video/x-ms-asf";
	(*this)[".asf"] = "video/x-ms-asf";
	(*this)[".wmv"] = "video/x-ms-wmv";
	(*this)[".avi"] = "video/x-msvideo";
}

#endif //MIME_HPP