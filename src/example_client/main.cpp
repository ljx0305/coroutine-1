#include <sys/epoll.h>

#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "coroutine.h"
#include "epoller.h"
#include "io.h"
#include "socket_util.h"
#include "util.h"

using namespace std;

struct UserParam
{
    std::string ip;
    uint16_t port;
    std::string file;
};

void SendCoroutine(void* args)
{
    int fd = SocketUtil::CreateTcpSocket();
    SocketUtil::SetBlock(fd, 0);

    UserParam* param = (UserParam*)args;

    string file = param->file;
    string ip = param->ip;
    uint16_t port = param->port;

    int ret = Connect(fd, ip, port);

    bool error = false;
    while (! error)
    {
        int file_fd = open(file.c_str(), O_RDONLY, 0664);

        if (file_fd < 0)
        {
            cout << "open " << file << " failed." << endl;
            cout << PrintErr("open", errno) << endl;
            break;
        }

        while (true)
        {
            uint8_t buf[1024*64];

            ret = read(file_fd, buf, sizeof(buf));
            
            if (ret > 0)
            {
                if (WriteGivenSize(fd, buf, ret) < 0)
                {
                    cout << "write " << ret << " bytes failed" << endl;
                    error = true;
                    break;
                }

                if (ReadGivenSize(fd, buf, ret) < 0)
                {
                    cout << "read " << ret << " bytes failed" << endl;
                    error = true;
                    break;
                }
            }
            else
            {
                break;
            }
        }
        close(file_fd);
    }
}

int main(int argc, char* argv[], char* env[])
{
    signal(SIGPIPE, SIG_IGN);

    if (argc < 4)
    {
        cout << "Usage ./main <ip> <port> <file>" << endl;
        return -1;
    }

    string ip = argv[1];
    uint16_t port = StrTo<uint16_t>(argv[2]);
    string file = argv[3];

    UserParam* param = new UserParam;
    param->ip = ip;
    param->port = port;
    param->file = file;
    CoroutineContext* ctx = CreateCoroutine("SendCoroutine", SendCoroutine, (void*)param);
    Resume(ctx);

    EventLoop();

    return 0;
}
