classdef RobotComms < handle
    %ROBOTCOMMS Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = private)
        name;
        channel;
        port;
        serial;
        
        TIMEOUT = 1.0; % (s)
        
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
        % msg is a string relating to the connection status
        function [s, msg] = connect(obj)

            % Build Bluetooth serial interface
            obj.port = Bluetooth(obj.name, obj.channel);
            obj.serial = ArduinoSerial(obj.port);
            
            % Connect to Bluetooth module
            try
                obj.serial.open();
                s = 1;
                msg = 'Connection successful.';
                return
            catch
                s = 0;
                msg = 'Bluetooth port open failed.';
                return
            end
        end
        
        % Sends begin message to robot
        % s is the connection status (1 = good, 0 = bad)
        % error is an empty string or an error message if s = 0
        function [s, error] = startRobot(obj)
            s = 0;
            error = '';
            
            % Send connect byte to robot
            obj.serial.flush();
            obj.serial.writeByte(obj.BYTE_CONNECT);
            if obj.serial.wait(1, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_CONNECT
                    error = 'Arduino Mega sent bad byte.';
                    return
                end
            else
                error = 'Arduino Mega timed out.';
                return
            end
        end
        
        % Sends drive voltages to robot and waits for response
        % s is connection status (1 = good, 0 = bad)
        % error is an empty string or an error message if s = 0
        function [s, error] = setDriveVoltage(obj, vL, vR)
            s = 0;
            error = '';
            
            % Send teleop command to robot
            obj.serial.writeByte(obj.BYTE_TELEOP);
            obj.serial.writeFloat(vL);
            obj.serial.writeFloat(vR);
            
            % Check response for errors
            if obj.serial.wait(1, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_TELEOP
                    error = 'Teleop response incorrect';
                    return
                end
            else
                error = 'Teleop response timeout';
                return
            end
            
            % If all went well
            s = 1;
        end
        
        % Gets odometry information from robot
        % s = 1 if everything worked
        function [odm, s, error] = getOdometryData(obj)
            s = 0;
            error = '';
            odm.x = 0;
            odm.y = 0;
            odm.h = 0;
            
            % Send odometry request to robot
            obj.serial.writeByte(obj.BYTE_ODOMETRY);
            
            % Check response for errors
            if obj.serial.wait(13, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_ODOMETRY
                    error = 'Odometry response incorrect';
                    return
                end
            else
                error = 'Odometry response timeout';
                return
            end
               
            % If all went well
            s = 1;
            odm.x = obj.serial.readFloat();
            odm.y = obj.serial.readFloat();
            odm.h = obj.serial.readFloat();
        end
        
        % Disconnects from bluetooth
        function disconnect(obj)
            if obj.serial.connected()
                obj.serial.writeByte(obj.BYTE_DISCONNECT);
                obj.serial.close();
            end
        end
        
        % Class destructor
        function delete(obj)
            obj.disconnect();
        end
    end
end

