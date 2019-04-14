import * as React from "react";
import {Utility} from "../../utility";
import * as Nexus from "nexusui";
import {
    PAGControlComponentColorSchemes,
    PAGControlComponentProps,
    PAGControlComponentState
} from "./pag-control-component";
import {ArrayExtended} from "../../array-extended";

interface PAGRotarySliderControlComponentProps extends PAGControlComponentProps {
}

export default class PAGRotarySliderControlComponent extends React.Component<PAGRotarySliderControlComponentProps, PAGControlComponentState> {
    static defaultProps = {
        value: 0,
        width: 50,
        height: 50,
        inverted: false,
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

    public componentDidMount() {
        let step = 0;
        if (this.props.steps.isNotEmpty()) {
            step = 1 / (this.props.steps.getLength() - 1);
        }

        this.nexusControl = new Nexus.Dial("#" + this.controlId, {
            size: [this.props.width, this.props.height],
            value: this.invertValueIfNeeded(this.props.value),
            interaction: "vertical",
            mode: "relative",
            step: step
        }).on("change", (newValue) => {
            this.props.valueChangedCallback(this.invertValueIfNeeded(newValue));
        }).on("click", this.props.beginGestureCallback)
            .on("release", this.props.endGestureCallback);
        this.nexusControl.position.sensitivity = 0.33;
        this.nexusControl.colorize("accent", this.props.colorScheme.accent);
        this.nexusControl.colorize("fill", this.props.colorScheme.fill);
    }

    public componentWillUnmount() {
        this.nexusControl.destroy();
    }

    public shouldComponentUpdate(newProps: PAGControlComponentProps, newState: PAGControlComponentState) {
        this.nexusControl.value = this.invertValueIfNeeded(newProps.value);
        return false;
    }

    public invertValueIfNeeded(value: number): number {
        if (this.props.inverted) {
            return 1 - value;
        }

        return value;
    }

    public render() {
        return <div className="parameterControl parameterControlRotarySlider" id={this.controlId}/>;
    }
}