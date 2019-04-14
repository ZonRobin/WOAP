import * as React from "react";
import * as PropTypes from "prop-types";
import RotarySlider from "../control-components/pag-rotary-slider-control-component";
import ControlComponentColorScheme from "../control-components/pag-control-component";
import {PluginParameterValueType} from "../../models/plugin-parameter-model";
import {ArrayExtended} from "../../array-extended";

interface PAGRotarySliderState {
}

interface PAGRotarySliderProps {
    size?: number;
    colorScheme?: ControlComponentColorScheme;
    inverted?: boolean;
}

export default class PAGRotarySlider extends React.Component<PAGRotarySliderProps, PAGRotarySliderState> {
    static contextTypes = {
        parameterValueSetterCallback: PropTypes.func,
        parameterValue: PropTypes.number,
        parameterValueType: PropTypes.string,
        parameterSteps: PropTypes.instanceOf(ArrayExtended),
        parameterBeginGestureCallback: PropTypes.func,
        parameterEndGestureCallback: PropTypes.func
    };

    static defaultProps = {
        size: 50,
        colorScheme: ControlComponentColorScheme.getDefault(),
        inverted: false
    };

    public shouldComponentUpdate(props, state, context) {
        return context.parameterValue !== this.context.parameterValue;
    }

    public render() {
        if (this.context.parameterValueType === PluginParameterValueType.Float) {
            return <RotarySlider valueChangedCallback={this.context.parameterValueSetterCallback}
                                 value={this.context.parameterValue}
                                 width={this.props.size}
                                 height={this.props.size}
                                 inverted={this.props.inverted}
                                 colorScheme={this.props.colorScheme}
                                 beginGestureCallback={this.context.parameterBeginGestureCallback}
                                 endGestureCallback={this.context.parameterEndGestureCallback}
            />;
        } else {
            return <RotarySlider valueChangedCallback={this.context.parameterValueSetterCallback}
                                 value={this.context.parameterValue}
                                 width={this.props.size}
                                 height={this.props.size}
                                 steps={this.context.parameterSteps}
                                 inverted={this.props.inverted}
                                 colorScheme={this.props.colorScheme}
                                 beginGestureCallback={this.context.parameterBeginGestureCallback}
                                 endGestureCallback={this.context.parameterEndGestureCallback}
            />;
        }
    }
}
