classdef RobotComms < handle
    %ROBOTCOMMS Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = private)
        name;
        channel;
        port;
        serial;
        
        BYTE_BEGIN = hex2dec('01');
        BYTE_UPDATE = hex2dec('03');
    end
    
    methods
        
        % Class constructor
        function obj = RobotComms(name, channel)
            obj.name = name;
            obj.channel = channel;
        end
        
        % Connects to Bluetooth module
        function connect(obj)
            obj.port = Bluetooth(obj.name, obj.channel);
            obj.serial = ArduinoSerial(obj.port);
            obj.serial.open();
        end
        
        % Tells robot to begin moving
        function begin(obj)
            obj.serial.writeByte(obj.BYTE_BEGIN);
        end
        
        % Gets robot data
        function data = getData(obj)
            obj.serial.writeByte(obj.BYTE_UPDATE);
            obj.serial.wait(24);
            data.time = obj.serial.readFloat();
            data.heading = obj.serial.readFloat();
            data.sonar.f = obj.serial.readFloat();
            data.sonar.b = obj.serial.readFloat();
            data.sonar.l = obj.serial.readFloat();
            data.sonar.r = obj.serial.readFloat();
        end
        
        % Disconnects from Bluetooth module
        function disconnect(obj)
            obj.serial.close();
        end
        
        % Class destructor
        function delete(obj)
            obj.disconnect();
        end
    end
end

