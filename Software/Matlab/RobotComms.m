classdef RobotComms < handle
    %ROBOTCOMMS Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = private)
        name;
        channel;
        port;
        serial;
        
        BYTE_CONNECT = hex2dec('01');
        BYTE_TELEOP = hex2dec('02');
        BYTE_ODOMETRY = hex2dec('03');
        BYTE_DISCONNECT = hex2dec('04');
    end
    
    methods
        
        % Class constructor
        function obj = RobotComms(name, channel)
            obj.name = name;
            obj.channel = channel;
        end
        
        % Connects to Robot
        % s is connection status (1 for good, 0 for bad)
        % msg is message containing details of connection status
        function [s, msg] = connect(obj)
            s = 0;
            
            % Build serial interface
            obj.port = Bluetooth(obj.name, obj.channel);
            obj.serial = ArduinoSerial(obj.port);
            
            % Connect to Bluetooth serial port
            try
                obj.serial.open();
            catch
                msg = 'Bluetooth port open failed.';
                return
            end
            
            % Communicate with Arduino Mega
            obj.serial.flush();
            obj.serial.writeByte(obj.BYTE_CONNECT);
            if obj.serial.wait(1, 1)
                if obj.serial.readByte() ~= obj.BYTE_CONNECT
                    msg = 'Arduino Mega sent bad byte.';
                    return
                end
            else
                msg = 'Arduino Mega timed out.';
                return
            end

            % If all went well
            s = 1;
            msg = 'Connection successful.';
            return
        end
        
        % Sends drive voltages to robot and waits for response
        % Returns 1 if response occured within 1 second
        function s = setDriveVoltage(obj, vL, vR)
            obj.serial.writeByte(obj.BYTE_TELEOP);
            obj.serial.writeFloat(vL);
            obj.serial.writeFloat(vR);
            obj.serial.wait(1, 1);
            if obj.serial.readByte() == obj.BYTE_TELEOP
                s = 1;
            else
                s = 0;
            end
        end
        
        % Gets odometry information from robot
        % s = 1 if everything worked
        function [odm, s] = getOdometryData(obj)
            obj.serial.writeByte(obj.BYTE_ODOMETRY);
            obj.serial.wait(13);
            if obj.serial.readByte() == obj.BYTE_ODOMETRY
                s = 1;
                odm.x = obj.serial.readFloat();
                odm.y = obj.serial.readFloat();
                odm.h = obj.serial.readFloat();
            else
                s = 0;
                odm = 0;
            end
        end
        
        % Disconnects from bluetooth
        function disconnect(obj)
            obj.serial.writeByte(obj.BYTE_DISCONNECT);
            obj.serial.close();
        end
        
        % Class destructor
        function delete(obj)
            obj.disconnect();
        end
    end
end

