classdef RobotComms < handle
    %ROBOTCOMMS Bluetooth communication system for RBE-2002 robot.
    %   Created by Dan Oates (RBE-2002 B17 Team 10).
    %   
    %   The robot communicates with Matlab over an Hc-06 Bluetooth module.
    %   They communicate via basic serial messages beginning with a byte
    %   conveying the message type followed by any included data. Timeouts
    %   exist on both the Matlab and robot ends to detect connection
    %   errors.
    %   
    %   See also: ROBOTDATA
    
    properties (Access = private, Constant)
        TIMEOUT = 1.0;  % Byte message timeout (s).
        
        % Message Type Bytes
        BYTE_CONNECT    = hex2dec('01');    % Connect & start robot
        BYTE_GETDATA    = hex2dec('02');    % Robot data requests
        BYTE_DISCONNECT = hex2dec('03');    % Disconnect
    end
    
    properties (Access = private)
        name;       % Name of Bluetooth module (string)
        channel;    % Bluetooth channel (number)
        port;       % Bluetooth serial port object
        serial;     % Arduino serial interface object
    end
    
    methods
        function obj = RobotComms(name, channel)
            % Constructs communication system over bluetooth module.
            % Inputs:
            %   name = name of bluetooth module
            %   channel = channel of bluetooth module
            obj.name = name;
            obj.channel = channel;
        end
        function [s, msg] = connect(obj)
            % Attempts connection to robot bluetooth module.
            % Inputs:
            %   s = connection status (1 for good, 0 for failure)
            %   msg = string message relating to connection status
            
            obj.port = Bluetooth(obj.name, obj.channel);
            obj.serial = ArduinoSerial(obj.port);
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
        function [rd, s, error] = getData(obj)
            % Requests data on robot (state, odometry, sonar, etc.)
            %   rd = RobotData class containing current robot data
            %   s = data response status (1 for ok, 0 for failure)
            %   error = '' or error message string relating to failure
            
            s = 0;
            error = '';
            
            % Request odometry data from robot
            obj.serial.writeByte(obj.BYTE_GETDATA);
            
            % Wait for data to return
            if obj.serial.wait(31, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_GETDATA
                    rd = 0;
                    error = 'Data response incorrect';
                    return
                end
            else
                rd = 0;
                error = 'Data response timeout';
                return
            end
               
            % If data was properly received
            s = 1;
            
            % Read Robot State
            stateByte = obj.serial.readByte();
            switch stateByte
                case  1, robotState = 'Searching for flame';
                case  2, robotState = 'Zeroing pan servo';
                case  3, robotState = 'Finding flame heading';
                case  4, robotState = 'Turning to flame heading';
                case  5, robotState = 'Driving to candle';
                case  6, robotState = 'Lowering tilt servo';
                case  7, robotState = 'Finding flame tilt';
                case  8, robotState = 'Aiming at flame';
                case  9, robotState = 'Extinguishing flame';
                case 10, robotState = 'Checking if flame is out';
                case 11, robotState = 'Backing away from candle';
                case 12, robotState = 'Turning back to wall';
                case 13, robotState = 'Going home';
                case 14, robotState = 'At home';
                otherwise, robotState = 'INVALID STATE';
            end
            
            % Deduce Flame Status
            if stateByte == 1
                flameStatus = 'Not Found';
            elseif stateByte >= 2
                if stateByte >= 12
                    flameStatus = 'Extinguished';
                else
                    flameStatus = 'Found';
                end
            end
            
            % Read Wall Follower State
            switch obj.serial.readByte()
                case 1, wallFollowerState = 'Stopped';
                case 2, wallFollowerState = 'Following left wall';
                case 3, wallFollowerState = 'Checking left side';
                case 4, wallFollowerState = 'Preparing for left turn';
                case 5, wallFollowerState = 'Turning left';
                case 6, wallFollowerState = 'Post turn';
                case 7, wallFollowerState = 'Backing from cliff';
                case 8, wallFollowerState = 'Turning right';
                otherwise, wallFollowerState = 'INVALID STATE';
            end
            
            % Read Remaining Information
            x = obj.serial.readFloat();
            y = obj.serial.readFloat();
            h = obj.serial.readFloat();
            sF = obj.serial.readFloat();
            sB = obj.serial.readFloat();
            sL = obj.serial.readFloat();
            sR = obj.serial.readFloat();
            flamePos = [...
                obj.serial.readFloat(); ...
                obj.serial.readFloat(); ...
                obj.serial.readFloat()];
            
            rd = RobotData(x, y, h, sF, sB, sL, sR, flamePos, ...
                robotState, wallFollowerState, flameStatus);
        end
        function disconnect(obj)
            % Sends stop message to robot then disconnects from Bluetooth.
            if isa(obj.serial, 'ArduinoSerial')
                if obj.serial.connected()
                    obj.serial.writeByte(obj.BYTE_DISCONNECT);
                    obj.serial.close();
                end
            end
        end
        function delete(obj)
            % Disconnects from robot before destructing.
            obj.disconnect();
        end
    end
end