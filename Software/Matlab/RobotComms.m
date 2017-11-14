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
        
        % Gets data on robot state (position, heading, sonar)
        % rd is robot data structure
        % s = 1 if everything worked
        function [rd, s, error] = getData(obj)
            s = 0;
            error = '';
            
            % Send odometry request to robot
            obj.serial.writeByte(obj.BYTE_ODOMETRY);
            
            % Check response for errors
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
               
            % If all went well
            s = 1;
            
            % Robot position vector (relative to start)
            rd.pos = [...
                obj.serial.readFloat(); ...
                obj.serial.readFloat()];
            
            % Robot heading (rad)
            rd.heading = obj.serial.readFloat();
            sh = sin(rd.heading);
            ch = cos(rd.heading);
            rotator = [ch sh; -sh ch];
            
            % Vectors from robot to sonar points
            rd.vRF = rotator * [0; +obj.serial.readFloat()];
            rd.vRB = rotator * [0; -obj.serial.readFloat()];
            rd.vRL = rotator * [-obj.serial.readFloat(); 0];
            rd.vRR = rotator * [+obj.serial.readFloat(); 0];
            
            % Vectors from ground to sonar points
            rd.vGF = rd.pos + rd.vRF;
            rd.vGB = rd.pos + rd.vRB;
            rd.vGL = rd.pos + rd.vRL;
            rd.vGR = rd.pos + rd.vRR;
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

