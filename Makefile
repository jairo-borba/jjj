CXX      := -c++
AR       := ar
ARFLAGS  := -rscv
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
LIB_DIR  := $(BUILD)/lib
TARGET   := libjjj.a
INCLUDE  := -Iinclude -I/home/jairo/myProjects/zlib-1.2.11
SRC      :=                                                    \
cpp/base/Identificable.cpp                                   \
cpp/appUtil/ApplicationLoader.cpp                            \
cpp/appUtil/ApplicationUnit.cpp                              \
cpp/appUtil/JJJException.cpp                                 \
cpp/appUtil/OSEnvironment.cpp                                \
cpp/appUtil/PrivateEnvironment.cpp                           \
cpp/base/Identificable.cpp                                   \
cpp/fileUtil/File.cpp                                        \
cpp/fileUtil/FileList.cpp                                    \
cpp/fileUtil/GZipInputStream.cpp                             \
cpp/fileUtil/GzipOutputstream.cpp                            \
cpp/fileUtil/GZipStream.cpp                                  \
cpp/fileUtil/InputFile.cpp                                   \
cpp/logging/Level.cpp                                        \
cpp/logging/LogClient.cpp                                    \
cpp/logging/Logger.cpp                                       \
cpp/logging/LoggersPool.cpp                                  \
cpp/logging/LogHandler.cpp                                   \
cpp/logging/LogMessagePreambule.cpp                          \
cpp/unixSystem/msgQueue/MessageQueue.cpp                     \
cpp/unixSystem/process/MultiProcess.cpp                      \
cpp/unixSystem/semaphore/BinarySemaphoreClient.cpp           \
cpp/unixSystem/semaphore/BinarySemaphoreServer.cpp           \
cpp/unixSystem/sharedMemory/Segment.cpp                      \
cpp/unixSystem/sharedMemory/SharedMemoryClient.cpp           \
cpp/unixSystem/sharedMemory/SharedMemory.cpp                 \
cpp/unixSystem/sharedMemory/SharedMemoryServer.cpp           \
cpp/unixSystem/signals/ChildProcessInformation.cpp           \
cpp/unixSystem/signals/SignalAction.cpp                      \
cpp/unixSystem/signals/SignalChild.cpp                       \
cpp/unixSystem/signals/SignalHandler.cpp                     \


OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(LIB_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(LIB_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $(LIB_DIR)/$(TARGET) $(OBJECTS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(LIB_DIR)/*


