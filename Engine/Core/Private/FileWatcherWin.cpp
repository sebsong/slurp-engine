#include "FileWatcher.h"

#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <thread>

namespace file_watcher {

    struct WatchContext {
        std::string path;
        FileEventCallback callback;
        HANDLE dirHandle;
        std::thread watchThread;
        bool running;
    };

    class WinFileWatcher : public FileWatcher {
    public:
        WinFileWatcher() : _running(false) {}

        ~WinFileWatcher() override {
            stop();
        }

        void addWatch(const std::string& path, const FileEventCallback& callback) override {
            std::lock_guard<std::mutex> lock(_watchMutex);

            if (_watchContexts.find(path) != _watchContexts.end()) {
                return; // Already watching
            }

            HANDLE dirHandle = CreateFileA(
                path.c_str(),
                FILE_LIST_DIRECTORY,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                nullptr,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                nullptr
            );

            if (dirHandle == INVALID_HANDLE_VALUE) {
                return;
            }

            WatchContext* context = new WatchContext{path, callback, dirHandle, {}, false};
            _watchContexts[path] = context;

            if (_running) {
                startWatchThread(context);
            }
        }

        void removeWatch(const std::string& path) override {
            std::lock_guard<std::mutex> lock(_watchMutex);

            auto it = _watchContexts.find(path);
            if (it != _watchContexts.end()) {
                WatchContext* context = it->second;
                context->running = false;

                if (context->watchThread.joinable()) {
                    context->watchThread.join();
                }

                CloseHandle(context->dirHandle);
                delete context;
                _watchContexts.erase(it);
            }
        }

        void pollEvents() override {
            std::lock_guard<std::mutex> lock(_queueMutex);

            while (!_eventQueue.empty()) {
                FileEvent event = _eventQueue.front();
                _eventQueue.pop();

                // Find callback for this event
                for (const auto& pair : _watchContexts) {
                    if (event.filePath.find(pair.first) == 0) {
                        pair.second->callback(event);
                        break;
                    }
                }
            }
        }

        void start() override {
            if (_running) {
                return;
            }

            _running = true;

            for (auto& pair : _watchContexts) {
                startWatchThread(pair.second);
            }
        }

        void stop() override {
            if (!_running) {
                return;
            }

            _running = false;

            for (auto& pair : _watchContexts) {
                WatchContext* context = pair.second;
                context->running = false;

                if (context->watchThread.joinable()) {
                    context->watchThread.join();
                }
            }
        }

        bool isRunning() const override {
            return _running;
        }

    private:
        void startWatchThread(WatchContext* context) {
            context->running = true;
            context->watchThread = std::thread([this, context]() {
                watchDirectory(context);
            });
        }

        void watchDirectory(WatchContext* context) {
            char buffer[4096];
            DWORD bytesReturned;
            OVERLAPPED overlapped = {0};
            overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

            while (context->running) {
                BOOL success = ReadDirectoryChangesW(
                    context->dirHandle,
                    buffer,
                    sizeof(buffer),
                    TRUE, // Watch subdirectories
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                    &bytesReturned,
                    &overlapped,
                    nullptr
                );

                if (!success) {
                    break;
                }

                DWORD waitResult = WaitForSingleObject(overlapped.hEvent, 500);
                if (waitResult != WAIT_OBJECT_0) {
                    continue;
                }

                if (!GetOverlappedResult(context->dirHandle, &overlapped, &bytesReturned, FALSE)) {
                    continue;
                }

                FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);

                while (true) {
                    wchar_t fileName[MAX_PATH];
                    wcsncpy_s(fileName, fni->FileName, fni->FileNameLength / sizeof(wchar_t));
                    fileName[fni->FileNameLength / sizeof(wchar_t)] = L'\0';

                    char fileNameAnsi[MAX_PATH];
                    WideCharToMultiByte(CP_UTF8, 0, fileName, -1, fileNameAnsi, MAX_PATH, nullptr, nullptr);

                    std::string fullPath = context->path + "\\" + fileNameAnsi;

                    FileEventType eventType = FileEventType::Modified;
                    switch (fni->Action) {
                        case FILE_ACTION_ADDED:
                            eventType = FileEventType::Created;
                            break;
                        case FILE_ACTION_REMOVED:
                            eventType = FileEventType::Deleted;
                            break;
                        case FILE_ACTION_MODIFIED:
                            eventType = FileEventType::Modified;
                            break;
                        case FILE_ACTION_RENAMED_OLD_NAME:
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            eventType = FileEventType::Renamed;
                            break;
                    }

                    FileEvent event{fullPath, eventType};

                    {
                        std::lock_guard<std::mutex> lock(_queueMutex);
                        _eventQueue.push(event);
                    }

                    if (fni->NextEntryOffset == 0) {
                        break;
                    }

                    fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                        reinterpret_cast<char*>(fni) + fni->NextEntryOffset
                    );
                }

                ResetEvent(overlapped.hEvent);
            }

            CloseHandle(overlapped.hEvent);
        }

        bool _running;
        std::unordered_map<std::string, WatchContext*> _watchContexts;
        std::mutex _watchMutex;
        std::queue<FileEvent> _eventQueue;
        std::mutex _queueMutex;
    };

    FileWatcher* createFileWatcher() {
        return new WinFileWatcher();
    }
}

#endif // PLATFORM_WINDOWS
