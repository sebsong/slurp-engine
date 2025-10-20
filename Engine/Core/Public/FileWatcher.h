#pragma once

#include <string>
#include <functional>
#include <vector>

namespace file_watcher {
    enum class FileEventType {
        Created,
        Modified,
        Deleted,
        Renamed
    };

    struct FileEvent {
        std::string filePath;
        FileEventType eventType;
    };

    typedef std::function<void(const FileEvent&)> FileEventCallback;

    class FileWatcher {
    public:
        virtual ~FileWatcher() = default;

        // Add a directory to watch (recursive)
        virtual void addWatch(const std::string& path, const FileEventCallback& callback) = 0;

        // Remove a watch
        virtual void removeWatch(const std::string& path) = 0;

        // Process file system events (call this in main loop)
        virtual void pollEvents() = 0;

        // Start/stop watching
        virtual void start() = 0;
        virtual void stop() = 0;

        virtual bool isRunning() const = 0;
    };

    // Factory function to create platform-specific watcher
    FileWatcher* createFileWatcher();
}
