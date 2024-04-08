TARGET:=tuyad_esp
CFILES:=$(wildcard *.c)

CFLAGS += -I$(HOME)/tuya-iot-core-sdk/include -I$(HOME)/tuya-iot-core-sdk/interface -I$(HOME)/tuya-iot-core-sdk/utils
LD_FLAGS += -L$(HOME)/tuya-iot-core-sdk/build/lib -Wl,-rpath=$(HOME)/tuya-iot-core-sdk/build/lib -llink_core -lutils_modules -lubox -lubus -lmiddleware_implementation -lblobmsg_json
#LD_FLAGS += -llink_core -lutils_modules -lubox -lubus -lmiddleware_implementation -lblobmsg_json -largp-standalone
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(CFILES)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(TARGET) $(CFILES) $(LD_FLAGS)

clean:
	rm -f $(TARGET)