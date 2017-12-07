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
        name;       % Name of Bluetooth module (string)
        channel;    % Bluetooth channel (number)
        port;       % Bluetooth serial port object
        serial;     % Arduino serial interface object
    end
    
    properties (Access = private, Constant)
        TIMEOUT = 1.0;  % Byte message timeout (s).
        
        % Message Type Bytes
        BYTE_CONNECT    = hex2dec('01');    % Connect & start robot
        BYTE_GETDATA    = hex2dec('02');    % Robot data requests
        BYTE_DISCONNECT = hex2dec('03');    % Disconnect
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
        function [rd, s, error] = getData(obj)
            % Requests data on robot state (position, heading, sonar, etc.)
            %   rd = RobotData class containing current robot data
            %   s = data response status (1 for ok, 0 for failure)
            %   error = '' or error message string relating to failure
            s = 0;
            error = '';
            
            % Request odometry data from robot
            obj.serial.writeByte(obj.BYTE_GETDATA);
            
            % Wait for data to return
            if obj.serial.wait(32, obj.TIMEOUT)
                if obj.serial.readByte() ~= obj.BYTE_GETDATA
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
            
            % Read Robot State
            switch obj.serial.readByte()
                case 1, robotState = 'Finding fire';
                case 2, robotState = 'Extinguishing fire';
                case 3, robotState = 'Returning home';
                case 4, robotState = 'At home';
            end
            
            % Read Flame Finder State
            switch obj.serial.readByte()
                case 1, flameFinderState = 'Scanning field';
                case 2, flameFinderState = 'Preparing for pan sweep';
                case 3, flameFinderState = 'Waiting for robot stop';
                case 4, flameFinderState = 'Finding flame pan';
                case 5, flameFinderState = 'Preparing for tilt sweep';
                case 6, flameFinderState = 'Finding flame tilt';
                case 7, flameFinderState = 'Aiming fan at flame';
                case 8, flameFinderState = 'Extinguishing flame';
                case 9, flameFinderState = 'Idle';
            end
            
            % Read Wall Follower State
            switch obj.serial.readByte()
                case 1, wallFollowerState = 'Stopped';
                case 2, wallFollowerState = 'Forward';
                case 3, wallFollowerState = 'Checking left side';
                case 4, wallFollowerState = 'Preparing for left turn';
                case 5, wallFollowerState = 'Turning left';
                case 6, wallFollowerState = 'Post turn';
                case 7, wallFollowerState = 'Backing from cliff';
                case 8, wallFollowerState = 'Turning right';
            end
            
            % Read Mathematical Information
            x = obj.serial.readFloat();
            y = obj.serial.readFloat();
            h = obj.serial.readFloat();
            sF = obj.serial.readFloat();
            sB = obj.serial.readFloat();
            sL = obj.serial.readFloat();
            sR = obj.serial.readFloat();
            
            rd = RobotData(x, y, h, sF, sB, sL, sR, ...
                robotState, flameFinderState, wallFollowerState);
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