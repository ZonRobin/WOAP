import * as React from "react";
import {Utility} from "../../utility";
import * as Nexus from "nexusui";
import {
    PAGControlComponentColorSchemes,
    PAGControlComponentProps,
    PAGControlComponentState
} from "./pag-control-component";
import {ArrayExtended} from "../../array-extended";

interface PAGLinearSliderControlComponentProps extends PAGControlComponentProps {
    valueBar?: boolean;
}

export default class PAGLinearSliderControlComponent extends React.Component<PAGLinearSliderControlComponentProps, PAGControlComponentState> {
    static defaultProps = {
        value: 0,
        width: 20,
        height: 120,
        inverted: false,
        valueBar: true,
        steps: new ArrayExtended<number>(),
        colorScheme: PAGControlComponentColorSchemes.grey,
        valueChangedCallback: (value: number) => {
        },
        beginGestureCallback: () => {
        },
        endGestureCallback: () => {
        }
    };
    protected controlId: string;
    protected nexusControl: any;

    public constructor(props) {
        super(props);
        this.controlId = "control-" + Utility.randDigits(10);
    }

    public hasValueBar() {
        return this.props.valueBar;
    }

    public componentDidMount() {
        let step = 0;
        if (this.hasValueBar()) {
            if (this.props.steps.isNotEmpty()) {
                step = 1 / (this.props.steps.getLength() - 1);
            }
        } else {
            if (this.props.steps.isNotEmpty()) {
                step = 2 / (this.props.steps.getLength() - 1);
            }
        }

        let sensitivity = 0.33;
        if (this.props.steps.getLength() === 2) {
            sensitivity = 5;
        }

        if (this.hasValueBar()) {
            this.nexusControl = new Nexus.Slider("#" + this.controlId, {
                size: [this.props.width, this.props.height],
                value: this.invertValueIfNeeded(this.props.value),
                step: step
            }).on("change", (newValue) => {
                newValue = this.invertValueIfNeeded(newValue);
                if (newValue !== this.props.value) {
                    this.props.valueChangedCallback(newValue);
                }
            });
        } else {
            this.nexusControl = new Nexus.Pan("#" + this.controlId, {
                mode: "relative",
                size: [this.props.width, this.props.height],
                value: this.fromNormalisedValue(this.props.value),
                step: step
            }).on("change", (valObject) => {
                let newValue = this.toNormalisedValue(valObject.value);
                if (newValue !== this.props.value) {
                    this.props.valueChangedCallback(newValue);
                }
            });
        }

        this.nexusControl
            .on("click", this.props.beginGestureCallback)
            .on("release", this.props.endGestureCallback);

        this.nexusControl.colorize("accent", this.props.colorScheme.accent);
        this.nexusControl.colorize("fill", this.props.colorScheme.fill);
        this.nexusControl.position.sensitivity = sensitivity;
    }

    public componentWillUnmount() {
        this.nexusControl.destroy();
    }

    public shouldComponentUpdate(newProps: PAGControlComponentProps, newState: PAGControlComponentState) {
        if (this.hasValueBar()) {
            this.nexusControl.value = this.invertValueIfNeeded(newProps.value);
        } else {
            this.nexusControl.value = this.fromNormalisedValue(newProps.value);
        }

        return false;
    }

    public invertValueIfNeeded(value: number): number {
        if (this.props.inverted) {
            return 1 - value;
        }

        return value;
    }

    public toNormalisedValue(val: number): number {
        if (this.props.inverted) {
            return (1 - val) / 2;
        } else {
            return (val + 1) / 2;
        }
    }

    public fromNormalisedValue(val: number): number {
        if (this.props.inverted) {
            return -val * 2 + 1;
        } else {
            return val * 2 - 1;
        }
    }

    public render() {
        return <div className="parameterControl parameterControlLinearSlider" id={this.controlId}></div>;
    }
}