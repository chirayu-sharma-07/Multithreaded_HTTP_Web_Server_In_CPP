#include<iostream>
#include<map>
#include<forward_list>
#include<string.h>
#include<unistd.h>

#ifdef _WIN32
#include<windows.h>
#endif

#ifdef linux
#include<arpa/inet.h>
#include<sys/socket.h>
#endif

using namespace std;


// Chirayu (The Creator of HTTP Web Server)

class StringUtility
{
    private:
    StringUtility();
    public:
    static void toLowerCase(char *str)
    {
        if(str==NULL) return;
        for(;*str;str++) if(*str>=65 && *str<=90) *str+=32;
    }
};

class HttpErrorStatusUtility
{
    private:
    HttpErrorStatusUtility(){}
    public:
    static void sendBadRequestError(int clientSocketDescriptor)
    {
        // will do later on
    }
    static void sendHttpVersionNotSupportedError(int clientSocketDescriptor, char *httpVersion)
    {
        // will do later on
    }
    static void sendNotFoundError(int clientSocketDescriptor, char *requestURI)
    {
        // will optimize this code later on
        char content[1000];
        char header[200];
        char response[1200];
        sprintf(content,"<!doctype html><html lang='en'><head><meta charset='utf-8'><title>404 Error</title></head><body>Requested Resource [%s] Not Found</body>", requestURI);
        int contentLength = strlen(content);
        sprintf(header,"HTTP/1.1 404 Not Found\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);
        strcpy(response,header);
        strcat(response,content);
        send(clientSocketDescriptor, response, strlen(response),0);
    }
    static void sendMethodNotAllowedError(int clientSocketDescriptor, char *method, char *requestURI)
    {
        // will do later on
    }
};

class Validator
{
    private:

    Validator(){} // Constructor in private so that this class cannot be instanciated

    public:

    static bool isValidMIMEType(string &mimeType)
    {
        return true;
    }

    static bool isValidPath(string &path)
    {
        return true;
    }

    static bool isValidURLFormat(string &url)
    {
        return true;
    }
};

class Error
{
    private:

    string error;

    public:

    Error(string error)
    {
        this->error = error;
    }

    bool hasError()
    {
        return (this->error.length() > 0);
    }

    string getError()
    {
        return this->error;
    }
};

class Request
{
    private:
    char *method;
    char *requestURI;
    char *httpVersion;
    Request(char *method, char *requestURI, char *httpVersion)
    {
        this->method = method;
        this->requestURI = requestURI;
        this->httpVersion = httpVersion;
    }
    friend class Bro;
};

class Response
{
    private:

    string contentType;
    forward_list<string> content;
    forward_list<string>::iterator contentIterator;
    unsigned long contentLength;

    public:

    Response()
    {
        this->contentLength = 0;
        this->contentIterator = this->content.before_begin();
    }

    ~Response()
    {

    }

    void setContentType(string contentType)
    {
        if(Validator::isValidMIMEType(contentType))
        {
            this->contentType = contentType;
        }
        else
        {

        }
    }

    Response & operator<<(string content)
    {
        this->contentLength += content.length();
        this->contentIterator = this->content.insert_after(this->contentIterator, content);
        return *this;
    }
    friend class HttpResponseUtility;
};

class HttpResponseUtility
{
    private:
    HttpResponseUtility(){}
    public:
    static void sendResponse(int clientSocketDescriptor, Response &response)
    {
        char header[200];
        int contentLength=response.contentLength;
        sprintf(header,"HTTP/1.1 200 OK\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);
        send(clientSocketDescriptor,header,strlen(header),0);
        auto contentIterator = response.content.begin();
        while(contentIterator!=response.content.end())
        {
            string str = *contentIterator;
            send(clientSocketDescriptor,str.c_str(),str.length(),0);
            ++contentIterator;
        }
    }
};

enum __request_method__{__GET__, __POST__, __PUT__, __DELETE__, __CONNECT__, __HEAD__, __OPTIONS__, __TRACE__};

typedef struct __url__mapping
{
__request_method__ requestMethod;
void (*mappedFunction)(Request &, Response &);
}URLMapping;

class Bro
{
    private:

    string staticResourcesFolder;
    map<string, URLMapping> urlMappings;

    public:

    Bro()
    {

    }

    ~Bro()
    {

    }

    void setStaticResourcesFolder(string staticResourcesFolder)
    {
        if(Validator::isValidPath(staticResourcesFolder))
        {
            this->staticResourcesFolder = staticResourcesFolder;
        }
        else
        {

        }
    }

    void get(string url, void (*callBack)(Request &, Response &))
    {
        if(Validator::isValidURLFormat(url))
        {
            urlMappings.insert(pair<string, URLMapping>(url, {__GET__, callBack}));
        }
    }

    void listen(int portNumber, void (*callBack)(Error &))
    {
        #ifdef _WIN32
        WSADATA wsaData;
        WORD ver;
        ver = MAKEWORD(1, 1);
        WSAStartup(ver, &wsaData);
        #endif

        int serverSocketDescriptor;
        char requestBuffer[4097]; // one extra byte for \0
        int requestLength;
        int x;
        serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(serverSocketDescriptor < 0)
        {
            #ifdef _WIN32
            WSACleanup();
            #endif
            
            Error error("Unable to create socket");
            callBack(error);
            return;
        }
        struct sockaddr_in serverSocketInformation;
        serverSocketInformation.sin_family = AF_INET;
        serverSocketInformation.sin_port = htons(portNumber);
        serverSocketInformation.sin_addr.s_addr = htonl(INADDR_ANY);
        int successCode = bind(serverSocketDescriptor, (struct sockaddr *)&serverSocketInformation, sizeof(serverSocketInformation));
        if(successCode < 0)
        {
            close(serverSocketDescriptor);

            #ifdef _WIN32
            WSACleanup();
            #endif

            char a[101];
            sprintf(a, "Unable to bind socket to port : %d", portNumber);
            Error error(a);
            callBack(error);
            return;
        }
        successCode = ::listen(serverSocketDescriptor, 10); // ::listen (By specifying this i am talking about the global listen function)
        if(successCode < 0)
        {
            close(serverSocketDescriptor);

            #ifdef _WIN32
            WSACleanup();
            #endif
            
            Error error("Unable to accept client connections");
            callBack(error);
            return;
        }
        Error error("");
        callBack(error);
        struct sockaddr_in clientSocketInformation;
        
        #ifdef linux
        socklen_t len = sizeof(clientSocketInformation);
        #endif

        #ifdef _WIN32
        int len = sizeof(clientSocketInformation);
        #endif

        int clientSocketDescriptor;
        while(true)
        {
            clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientSocketInformation, &len);
            requestLength = recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer)-sizeof(char),0);
            if(requestLength==0 || requestLength==-1)
            {
                close(clientSocketDescriptor);
                continue;
            }
            int e;
            char *method, *requestURI, *httpVersion;
            requestBuffer[requestLength] = '\0';
            // code to parse the first line of the http request starts here
            // first line should be REQUEST_METHOD space URI space HTTPVersionCFRLF
            method = requestBuffer;
            e = 0;
            while(requestBuffer[e] && requestBuffer[e]!=' ') e++;
            if(requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestBuffer[e] = '\0';
            e++;
            if(requestBuffer[e]==' ' || requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            StringUtility::toLowerCase(method);

            if(!(strcmp(method,"get")==0 || strcmp(method,"post")==0 ||strcmp(method,"put")==0 ||strcmp(method,"delete")==0 ||strcmp(method,"head")==0 ||strcmp(method,"options")==0 ||strcmp(method,"trace")==0 ||strcmp(method,"connect")==0))
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestURI=requestBuffer + e;
            while(requestBuffer[e] && requestBuffer[e]!=' ') e++;
            if(requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestBuffer[e]='\0';
            e++;
            if(requestBuffer[e]==' ' || requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            httpVersion=requestBuffer + e;
            while(requestBuffer[e] && requestBuffer[e] != '\r' && requestBuffer[e]!='\n') e++;
            if(requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            if(requestBuffer[e]=='\r' && requestBuffer[e + 1]!='\n')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            if(requestBuffer[e]=='\r')
            {
                requestBuffer[e]='\0';
                e = e + 2;
            }
            else
            {
                requestBuffer[e]='\0';
                e = e + 1;
            }
            StringUtility::toLowerCase(httpVersion);
            if(strcmp(httpVersion,"http/1.1")!=0)
            {
                HttpErrorStatusUtility::sendHttpVersionNotSupportedError(clientSocketDescriptor,httpVersion);
                close(clientSocketDescriptor);
                continue;
            }
            auto urlMappingsIterator=urlMappings.find(requestURI);
            if(urlMappingsIterator==urlMappings.end())
            {
                HttpErrorStatusUtility::sendNotFoundError(clientSocketDescriptor, requestURI);
                close(clientSocketDescriptor);
                continue;
            }
            URLMapping urlMapping = urlMappingsIterator->second;
            if(urlMapping.requestMethod==__GET__ && strcmp(method,"get")!=0)
            {
                HttpErrorStatusUtility::sendMethodNotAllowedError(clientSocketDescriptor,method,requestURI);
                close(clientSocketDescriptor);
                continue;
            }
            // code to parse the header and then the payload if exists starts here
            // code to parse the header and then the payload if exists ends here
            Request request(method,requestURI,httpVersion);
            Response response;
            urlMapping.mappedFunction(request,response);
            HttpResponseUtility::sendResponse(clientSocketDescriptor,response);
            close(clientSocketDescriptor);
            // lot of code will be written here later on
        } // Infinite loop ends here

        #ifdef _WIN32
        WSACleanup();
        #endif
    }
};




// Sam (The User of HTTP Web Server)

int main()
{
    Bro bro;
    bro.setStaticResourcesFolder("whatever the folder name is");
    bro.get("/", [](Request &request, Response &response) -> void {
        const char *htmlPage = R""""(
        <!DOCTYPE html>
        <html lang = 'en'>
        <head>
        <meta charset = 'utf-8'>
        <title>whatever</title>
        </head>
        <body>
        <h1>Hey, Welcome</h1>
        <a href = 'getCustomers'>Customers</a>
        </body>
        </html>
        )"""";
        response.setContentType("text/html"); // Setting MIME Type
        response<<htmlPage;
    });

    bro.get("/getCustomers", [](Request &request, Response &response) -> void {
        const char *htmlPage = R""""(
        <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Dynamic Mesh Configuration</title>
      <style>
        * {
          margin: 0;
          padding: 0;
          box-sizing: border-box;
          font-family: Arial, sans-serif;
        }

        html, body {
          height: 100%;
          min-height: 100vh;
          background-color: #f9f9f9;
          display: flex;
          flex-direction: column;
        }

        header {
          background-color: #d21134;
          display: flex;
          justify-content: center;
          align-items: center;
          height: 60px;
          padding: 10px 20px;
        }

        header img {
          max-height: 40px;
        }

        .container {
          flex: 1;
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: flex-start;
          padding: 20px;
          max-width: 900px;
          margin: 0 auto;
          width: 100%;
        }

        h1 {
          color: #d21134;
          margin-bottom: 20px;
          font-size: 22px;
          text-align: center;
        }

        .buttons {
          display: flex;
          flex-wrap: wrap;
          justify-content: center;
          gap: 16px;
          width: 100%;
        }

        .card {
          background-color: white;
          color: #d21134;
          padding: 20px;
          border-radius: 10px;
          border: 2px solid #d21134;
          box-shadow: 0 4px 8px rgba(0,0,0,0.1);
          font-size: 16px;
          font-weight: bold;
          cursor: pointer;
          transition: all 0.3s ease;
          flex: 1 1 280px;
          max-width: 300px;
          text-align: center;
        }

        .card:hover {
          background-color: #d21134;
          color: white;
          transform: translateY(-3px);
          box-shadow: 0 8px 16px rgba(0,0,0,0.15);
        }

        footer {
          background-color: #f1f1f1;
          padding: 10px 0;
          text-align: center;
          color: #333;
          font-size: 14px;
          position: fixed;
          left: 0;
          bottom: 0;
          width: 100%;
          z-index: 1000;
        }

        @media (max-width: 480px) {
          .card {
            font-size: 14px;
            padding: 16px;
          }
        }
      </style>
    </head>
    <body>

    <div class=container>
    <h1>Welcome</h1>
    <div class=buttons>
    <div class=card onclick=location.href='/getParentChildList'>Get Parent Child List</div>
    <div class=card onclick=location.href='/massConfiguration'>Mass Configuration</div>
    <div class=card onclick=location.href='/downLinkConfiguration'>Down Link Configuration</div>
    <div class=card onclick=location.href='/assignManualUplink'>Assign Manual Uplink(s)</div>
    <div class=card onclick=location.href='/spiralMeshConfiguration'>Spiral Mesh Configuration</div>
    <div class=card onclick=location.href='/setFESensorCredentials'>Set FE Sensor Credentials</div>
    </div></div>
    </body>
    </html>
        )"""";
        response.setContentType("text/html"); // Setting MIME Type
        response<<htmlPage;
    });

    bro.listen(6060, [](Error &error) -> void {
        if(error.hasError())
        {
            cout<<error.getError()<<endl;
            return;
        }
        cout<<"Bro Server is successfully listening on port number 6060"<<endl;
    });
    return 0;
}