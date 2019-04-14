import * as React from "react";
import * as PropTypes from "prop-types";
import PluginParameterModel from "../../models/plugin-parameter-model";
import TrackPluginModel from "../../models/track-plugin-model";
import {ArrayExtended} from "../../array-extended";

export interface PAGParameterViewProps {
    parameterModel: PluginParameterModel;
}

export interface PAGParameterViewState {
    value: number;
    valueText: string;
}

export default class PAGParameterView extends React.Component<PAGParameterViewProps, PAGParameterViewState> {
    static childContextTypes = {
        parameterName: PropTypes.string,
        parameterValue: PropTypes.number,
        parameterValueText: PropTypes.string,
        parameterValueSetterCallback: PropTypes.func,
        parameterValueType: PropTypes.string,
        parameterSteps: PropTypes.instanceOf(ArrayExtended),
        parameterBeginGestureCallback: PropTypes.func,
        parameterEndGestureCallback: PropTypes.func
    };

    static contextTypes = {
        pluginModel: PropTypes.instanceOf(TrackPluginModel)
    };

    protected parameterControlValueChangedBound;
    protected beingHandled = false;
    protected beginGestureBound;
    protected endGestureBound;

    constructor(props, context) {
        super(props, context);

        let parameterModel = this.props.parameterModel;
        this.state = {
            value: parameterModel.getValue(),
            valueText: parameterModel.getValueText()
        };
        this.parameterControlValueChangedBound = this.parameterControlValueChanged.bind(this);
        this.beginGestureBound = this.onBeginGesture.bind(this);
        this.endGestureBound = this.onEndGesture.bind(this);
    }

    public componentWillMount(): void {
        this.props.parameterModel.addListener(this);
    }

    public componentWillUnmount() {
        this.props.parameterModel.removeListener(this);
    }

    public valueChanged(newValue: number, newValueText: string): void {
        if (this.beingHandled)
            this.setState({valueText: newValueText});
        else
            this.setState({value: newValue, valueText: newValueText});
    }

    public parameterControlValueChanged(newValue: number): void {
        this.props.parameterModel.setValue(newValue);
        this.setState({value: newValue});
    }

    public onBeginGesture() {
        this.beingHandled = true;
    }

    public onEndGesture() {
        this.beingHandled = false;
        this.setState({
            value: this.props.parameterModel.getValue(),
            valueText: this.props.parameterModel.getValueText()
        });
    }

    public shouldComponentUpdate(props, state, context) {
        return this.state.valueText !== state.valueText
            || this.state.value !== state.value
            || this.props.parameterModel !== props.parameterModel;
    }

    public getChildContext() {
        return {
            parameterName: this.props.parameterModel.getName(),
            parameterValue: this.state.value,
            parameterValueText: this.state.valueText,
            parameterValueSetterCallback: this.parameterControlValueChangedBound,
            parameterValueType: this.props.parameterModel.getValueType(),
            parameterSteps: this.props.parameterModel.getSteps(),
            parameterBeginGestureCallback: this.beginGestureBound,
            parameterEndGestureCallback: this.endGestureBound,
        };
    }

    public render() {
        return this.props.children;
    }
}
