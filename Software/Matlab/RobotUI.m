classdef RobotUI < handle
    %ROBOTUI User interface for RBE-2002 robot.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   This UI consists of:
    %       - A figure title
    %       - Button to connect to robot Bluetooth
    %       - Button to begin robot movement
    %       - Button to shut down and disconnect from robot
    %       - Button to replay last robot run
    %       - Plot of robot position and heading with wall estimations
    %   The update method continuously sets the figure to the right half of
    %   the screen, leaving room for the command window on the left half.
    %   
    %   See also: ROBOTDATA
    
    properties (Access = private)
        
        % Figure Constants
        figurePosition = [809, 49, 784, 768];   % On the screen
        mapLimits = [-2.5, +2.5, -2.5, +2.5];   % Field map limits (m)
        
        % UI Handles
        fig;        % Figure handle
        titleText;  % Figure title
        cButton;    % Connect button
        bButton;    % Begin button
        dButton;    % Disconnect button
        rButton;    % Replay button
        fieldAxis;  % Field map plot

        % Button Statuses
        cbPressed = 0;  % Connect
        bbPressed = 0;  % Begin
        dbPressed = 0;  % Disconnect
        rbPressed = 0;  % Replay
        
        % Mapping Aids
        robotRadius = 0.125;    % Approximate robot radius (m)
    end
    
    methods (Access = public)
        function obj = RobotUI()
            % Constructs and displays UI
            
            % Matlab Figure
            obj.fig = figure(...
                'Name', 'Robot Console', ...
                'Position', obj.figurePosition, ...
                'Resize', 'off');
            
            % Title Text
            obj.titleText = uicontrol(...
                'Style', 'text', ...
                'String', 'Robot Control Station', ...
                'BackgroundColor', [0.5, 0.5, 0.5], ...
                'FontSize', 24, ...
                'FontWeight', 'bold', ...
                'Units', 'normalized', ...
                'Position', obj.figPos([0.04 0.96 0.02 0.08]));
            
            % Pushbuttons
            obj.cButton = obj.makeButton('Connect', ...
                [0.04, 0.24, 0.10, 0.20], @obj.cbCallback);
            obj.bButton = obj.makeButton('Begin', ...
                [0.28, 0.48, 0.10, 0.20], @obj.bbCallback);
            obj.dButton = obj.makeButton('Disconnect', ...
                [0.52, 0.72, 0.10, 0.20], @obj.dbCallback);
            obj.rButton = obj.makeButton('Replay', ...
                [0.76, 0.96, 0.10, 0.20], @obj.rbCallback);

            % Field Axes (for plotting robot and field)
            obj.fieldAxis = axes(...
                'XGrid', 'on', ...
                'YGrid', 'on', ...
                'OuterPosition', obj.figPos([0.05 0.95 0.25 0.95]));
            
            % Set up empty field plot
            figure(obj.fig)
            obj.fig.CurrentAxes = obj.fieldAxis;
            title('Field Plot');
            xlabel('X Position (m)')
            ylabel('Y Position (m)')
            
            % Force immediate drawing of UI
            drawnow
        end
        function update(obj)
            % Redraws UI and resets pushbutton statuses
            obj.fig.Position = obj.figurePosition;
            axis equal
            axis(obj.mapLimits)
            grid on
            title('Field Plot');
            xlabel('X Position (m)')
            ylabel('Y Position (m)')
            
            % Reset pushbuttons
            obj.cbPressed = 0;
            obj.bbPressed = 0;
            obj.dbPressed = 0;
            obj.rbPressed = 0;
            
            % Force immediate drawing of UI
            drawnow
        end
        function [p] = connectButton(obj)
            % Returns 1 if button has been pressed since last UI update.
            p = obj.cbPressed;
            obj.cbPressed = 0;
        end
        function [p] = beginButton(obj)
            % Returns 1 if button has been pressed since last UI update.
            p = obj.bbPressed;
            obj.bbPressed = 0;
        end 
        function [p] = disconnectButton(obj)
            % Returns 1 if button has been pressed since last UI update.
            p = obj.dbPressed;
            obj.dbPressed = 0;
        end
        function [p] = replayButton(obj)
            % Returns 1 if button has been pressed since last UI update.
            p = obj.rbPressed;
            obj.rbPressed = 0;
        end
    end
    methods (Access = private)
        function [pf] = figPos(~, pi)
            % Converts my preferred figure position format to Matlab
            pf = zeros(1, 4);
            pf(1) = pi(1);
            pf(2) = 1 - pi(4);
            pf(3) = pi(2) - pi(1);
            pf(4) = pi(4) - pi(3);
        end
        function [button] = makeButton(obj, name, position, callback)
            % Builds one of the four UI buttons.
            button = uicontrol(...
                'Style', 'pushbutton', ...
                'String', name, ...
                'FontSize', 16, ...
                'Units', 'normalized', ...
                'Position', obj.figPos(position), ...
                'Callback', callback);  
        end
        function cbCallback(obj, ~, ~)
            % Sets connect button status to 1
            obj.cbPressed = 1;
        end 
        function bbCallback(obj, ~, ~)
            % Sets begin button status to 1
            obj.bbPressed = 1;
        end     
        function dbCallback(obj, ~, ~)
            % Sets disconnect button status to 1
            obj.dbPressed = 1;
        end
        function rbCallback(obj, ~, ~)
            % Sets replay button status to 1
            obj.rbPressed = 1;
        end
    end
end