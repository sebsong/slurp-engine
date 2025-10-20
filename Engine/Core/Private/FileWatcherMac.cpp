#include "FileWatcher.h"

#ifdef PLATFORM_MACOS

#include <CoreServices/CoreServices.h>
#include <unordered_map>
#include <mutex>
#include <queue>

namespace file_watcher {

    struct WatchContext {
        std::string path;
        FileEventCallback callback;
        std::queue<FileEvent>* eventQueue;
        std::mutex* queueMutex;
    };

    class MacFileWatcher : public FileWatcher {
    public:
        MacFileWatcher() : _running(false), _stream(nullptr) {}

        ~MacFileWatcher() override {
            stop();
            for (auto& pair : _watchContexts) {
                delete pair.second;
            }
        }

        void addWatch(const std::string& path, const FileEventCallback& callback) override {
            std::lock_guard<std::mutex> lock(_watchMutex);

            if (_watchContexts.find(path) != _watchContexts.end()) {
                return; // Already watching
            }

            WatchContext* context = new WatchContext{path, callback, &_eventQueue, &_queueMutex};
            _watchContexts[path] = context;

            // Restart stream if running
            if (_running) {
                stop();
                start();
            }
        }

        void removeWatch(const std::string& path) override {
            std::lock_guard<std::mutex> lock(_watchMutex);

            auto it = _watchContexts.find(path);
            if (it != _watchContexts.end()) {
                delete it->second;
                _watchContexts.erase(it);

                if (_running) {
                    stop();
                    start();
                }
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
            if (_running || _watchContexts.empty()) {
                return;
            }

            // Create array of paths to watch
            CFMutableArrayRef pathsToWatch = CFArrayCreateMutable(nullptr, _watchContexts.size(), &kCFTypeArrayCallBacks);

            for (const auto& pair : _watchContexts) {
                CFStringRef pathRef = CFStringCreateWithCString(nullptr, pair.first.c_str(), kCFStringEncodingUTF8);
                CFArrayAppendValue(pathsToWatch, pathRef);
                CFRelease(pathRef);
            }

            FSEventStreamContext streamContext = {0, this, nullptr, nullptr, nullptr};

            _stream = FSEventStreamCreate(
                nullptr,
                &MacFileWatcher::fsEventsCallback,
                &streamContext,
                pathsToWatch,
                kFSEventStreamEventIdSinceNow,
                0.3, // Latency in seconds
                kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagUseCFTypes
            );

            CFRelease(pathsToWatch);

            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdeprecated-declarations"
            FSEventStreamScheduleWithRunLoop(_stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
            #pragma clang diagnostic pop
            FSEventStreamStart(_stream);

            _running = true;
        }

        void stop() override {
            if (!_running || !_stream) {
                return;
            }

            FSEventStreamStop(_stream);
            FSEventStreamInvalidate(_stream);
            FSEventStreamRelease(_stream);
            _stream = nullptr;
            _running = false;
        }

        bool isRunning() const override {
            return _running;
        }

    private:
        static void fsEventsCallback(
            ConstFSEventStreamRef streamRef,
            void* clientCallBackInfo,
            size_t numEvents,
            void* eventPaths,
            const FSEventStreamEventFlags eventFlags[],
            const FSEventStreamEventId eventIds[]
        ) {
            MacFileWatcher* watcher = static_cast<MacFileWatcher*>(clientCallBackInfo);
            CFArrayRef paths = static_cast<CFArrayRef>(eventPaths);

            for (size_t i = 0; i < numEvents; i++) {
                CFStringRef pathRef = static_cast<CFStringRef>(CFArrayGetValueAtIndex(paths, i));
                char path[PATH_MAX];
                CFStringGetCString(pathRef, path, PATH_MAX, kCFStringEncodingUTF8);

                FileEventType eventType = FileEventType::Modified;
                FSEventStreamEventFlags flags = eventFlags[i];

                if (flags & kFSEventStreamEventFlagItemCreated) {
                    eventType = FileEventType::Created;
                } else if (flags & kFSEventStreamEventFlagItemRemoved) {
                    eventType = FileEventType::Deleted;
                } else if (flags & kFSEventStreamEventFlagItemRenamed) {
                    eventType = FileEventType::Renamed;
                } else if (flags & kFSEventStreamEventFlagItemModified) {
                    eventType = FileEventType::Modified;
                }

                FileEvent event{std::string(path), eventType};

                std::lock_guard<std::mutex> lock(*watcher->_watchContexts.begin()->second->queueMutex);
                watcher->_eventQueue.push(event);
            }
        }

        bool _running;
        FSEventStreamRef _stream;
        std::unordered_map<std::string, WatchContext*> _watchContexts;
        std::mutex _watchMutex;
        std::queue<FileEvent> _eventQueue;
        std::mutex _queueMutex;
    };

    FileWatcher* createFileWatcher() {
        return new MacFileWatcher();
    }
}

#endif // PLATFORM_MACOS
