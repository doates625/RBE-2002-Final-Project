classdef RobotComms < handle
    %ROBOTCOMMS Bluetooth communication system for RBE-2002 robot.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   The robot communicates with Matlab over an Hc-06 Bluetooth module.
    %   They communicate via basic serial messages beginning with a byte
    %   conveying the message type followed by any included data. Timeouts
    %   exist on both the Matlab and robot ends to detect connection
    %   errors.
    %   
    %   See also: ROBOTDATA
    
    properties (Access = private)
        % Bluetooth Communication
        name;       % Name of Bluetooth module (string)
        channel;    % Bluetooth channel (number)
        port;       % Bluetooth serial port object
        serial;     % Arduino serial interface object
        
        % Serial Protocol
        TIMEOUT = 1.0;                      % Byte message timeout (s).
        BYTE_CONNECT    = hex2dec('01');    % Connect & start robot
        BYTE_TELEOP     = hex2dec('02');    % Teleop commands
        BYTE_ODOMETRY   = hex2dec('03');    % Odometry requests
        BYTE_DISCONNECT = hex2dec('04');    % Disconnect
    end
    
    methods
        function obj = RobotComms(name, channel)
            % Constructs communication system over bluetooth module.
            %   name = name of bluetooth module
            %   channel = channel of bluetooth module
            obj.name = name;
            obj.channel = channel;
        end
        function [s, msg] = connect(obj)
            % Attempts connection to robot bluetooth module.
            %   s = connection status (1 for good, 0 for failure)
            %   msg = string message relating to connection status
            obj.port = Bluetooth(obj.name, obj.channel);
            obj.serial = ArduinoSerial(obj.port);
            
            % Attempt connection
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
        function [s, error] = start(obj)
            % Sends begin message to robot and waits for acknowledge.
            %   s = acknowledge status (1 for success, 0 for failure)
            %   error = '' or error message string if connection failed
            s = 0;
            error = '';
            
            % Send begin command to robot
            obj.serial.writeByte(obj.BYTE_CONNECT);
            
            % Wait for acknowledge
            if obj.serial.wait(1, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_CONNECT
                    error = 'Arduino Mega sent bad byte.';
                    return
                end
            else
                error = 'Arduino Mega timed out.';
                return
            end
            
            % If all went well
            s = 1;
        end
        function [s, error] = drive(obj, voltage, heading)
            % Sends teleop drive command to robot
            %   voltage = forward drive voltage (negative for reverse)
            %   heading = target heading to face towards
            %   s = acknowledge status (1 for ok, 0 for failure)
            %   error = '' or error message string relating to failure
            s = 0;
            error = '';
            
            % Send teleop command to robot
            obj.serial.writeByte(obj.BYTE_TELEOP);
            obj.serial.writeFloat(voltage);
            obj.serial.writeFloat(deg2rad(heading));
            
            % Wait for acknowledge
            if obj.serial.wait(1, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_TELEOP
                    error = 'Drive response incorrect';
                    return
                end
            else
                error = 'Drive response timeout';
                return
            end
            
            % If all went well
            s = 1;
        end
        function [rd, s, error] = getData(obj)
            % Requests data on robot state (position, heading, sonar, etc.)
            %   rd = RobotData class containing current robot data
            %   s = data response status (1 for ok, 0 for failure)
            %   error = '' or error message string relating to failure
            s = 0;
            error = '';
            
            % Request odometry data from robot
            obj.serial.writeByte(obj.BYTE_ODOMETRY);
            
            % Wait for data to return
            if obj.serial.wait(29, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_ODOMETRY
                    rd = 0;
                    error = 'Odometry response incorrect';
                    return
                end
            else
                rd = 0;
                error = 'Odometry response timeout';
                return
            end
               
            % Read robot data if all went well
            s = 1;
            x = obj.serial.readFloat();
            y = obj.serial.readFloat();
            h = obj.serial.readFloat();
            sF = obj.serial.readFloat();
            sB = obj.serial.readFloat();
            sL = obj.serial.readFloat();
            sR = obj.serial.readFloat();
            rd = RobotData(x, y, h, sF, sB, sL, sR);
        end
        function disconnect(obj)
            % Sends stop message to robot then disconnects from Bluetooth.
            if obj.serial.connected()
                obj.serial.writeByte(obj.BYTE_DISCONNECT);
                obj.serial.close();
            end
        end
        function delete(obj)
            % Disconnects from robot before destructing.
            obj.disconnect();
        end
    end
end
