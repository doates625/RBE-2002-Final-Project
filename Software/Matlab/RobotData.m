classdef RobotData
    %ROBOTDATA Data from firefighter robot at an instant in time.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   This class is used in array form to log the data from the robot
    %   over an entire run for replay and post-processing purposes.
    %
    %   All units are standard SI (meters, seconds, radians, etc), and
    %   all vectors are in [x; y] format.
    
    properties
        pos = [0; 0];   % Robot position from start
        heading = 0;    % Robot heading (0 - 2pi)
        
        sFvalid = 0;    % Sonar front valid
        sBvalid = 0;    % Sonar back valid
        sLvalid = 0;    % Sonar left valid
        sRvalid = 0;    % Sonar right valid

        sonarF = [0; 0];    % Sonar position front
        sonarB = [0; 0];    % Sonar position back
        sonarL = [0; 0];    % Sonar position left
        sonarR = [0; 0];    % Sonar position right
    end
    
    methods
        function obj = RobotData(x, y, h, sF, sB, sL, sR)
            % Constructs RobotData from given robot data.
            %   x = x-position (m)
            %   y = y-position (m)
            %   h = heading (rad)
            %   sF = front sonar distance (m)
            %   sB = back sonar distance (m)
            %   sL = left sonar distance (m)
            %   sR = right sonar distance (m)
            obj.pos = [x; y];
            obj.heading = h;
            
            obj.sFvalid = (sF ~= 0);
            obj.sBvalid = (sB ~= 0);
            obj.sLvalid = (sL ~= 0);
            obj.sRvalid = (sR ~= 0);
            
            sh = sin(h);
            ch = cos(h);
            rh = [ch sh; -sh ch];
            
            obj.sonarF = obj.pos + rh * [0; +sF];
            obj.sonarB = obj.pos + rh * [0; -sB];
            obj.sonarL = obj.pos + rh * [-sL; 0];
            obj.sonarR = obj.pos + rh * [+sR; 0];
        end
        function plot(obj)
            % Plots robot data on current axes.
            
            % Plot robot position and heading
            plot(obj.pos(1), obj.pos(2), 'o', 'color', 'b')
            hVec = 0.125 * [sin(obj.heading); cos(obj.heading)];
            plot(obj.pos(1) + [0 hVec(1)], obj.pos(2) + [0 hVec(2)], ...
                'color', 'b', 'linewidth', 2)

            % Plot sonar vectors from robot to walls
            plot([obj.pos(1) obj.sonarF(1)], ...
                 [obj.pos(2) obj.sonarF(2)], ...
                 '--', 'color', 'k')
            plot([obj.pos(1) obj.sonarB(1)], ...
                 [obj.pos(2) obj.sonarB(2)], ...
                 '--', 'color', 'k')
            plot([obj.pos(1) obj.sonarL(1)], ...
                 [obj.pos(2) obj.sonarL(2)], ...
                 '--', 'color', 'k')
            plot([obj.pos(1) obj.sonarR(1)], ...
                 [obj.pos(2) obj.sonarR(2)], ...
                 '--', 'color', 'k')
        end
    end
end