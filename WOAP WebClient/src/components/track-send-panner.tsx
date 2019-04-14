import * as React from "react";
import Panner from "./control-components/panner";
import TrackSendModel, {TrackSendModelListener} from "../models/track-send-model";
import Color from "../helpers/color/color";

interface TrackSendPannerProps {
    model: TrackSendModel;
}

interface TrackSendPannerState {
    panLevel: number;
}

export default class TrackSendPanner extends React.Component<TrackSendPannerProps, TrackSendPannerState> implements TrackSendModelListener {
    protected isBeingDragged: boolean = false;

    protected dragStartBound;
    protected dragEndBound;
    protected valueChangedCallbackBound;

    constructor(props) {
        super(props);

        this.state = {
            panLevel: this.props.model.getPan()
        };

        this.dragStartBound = this.dragStart.bind(this);
        this.dragEndBound = this.dragEnd.bind(this);
        this.valueChangedCallbackBound = this.valueChangedCallback.bind(this);
    }

    public componentWillMount() {
        this.props.model.addListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeListener(this);
    }

    public gainChanged(newValue: number): void {
    }

    public panChanged(newValue: number): void {
        if (!this.isBeingDragged)
            this.setState({panLevel: newValue});
    }

    public colorChanged(newColor: Color): void {
    }

    public nameChanged(newName: string): void {
    }

    public audioSourcePositionChanged(newPosition: number): void {
    }

    public sendWillBeDeleted(): void {
    }

    protected valueChangedCallback(newValue: number) {
        this.props.model.setPan(newValue);
        this.setState({panLevel: newValue});
    }

    public shouldComponentUpdate(props: TrackSendPannerProps, state: TrackSendPannerState) {
        return this.state.panLevel !== state.panLevel
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackSendPannerProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            newProps.model.addListener(this);
            this.setState({panLevel: newProps.model.getPan()});
        }
    }

    public dragStart() {
        this.isBeingDragged = true;
    }

    public dragEnd() {
        this.isBeingDragged = false;
        this.setState({panLevel: this.props.model.getPan()});
    }

    public render() {
        return (
            <div className="trackSendPanner">
                <Panner
                    value={this.state.panLevel}
                    valueChangedCallback={this.valueChangedCallbackBound}
                    minValue={-1}
                    maxValue={1}
                    isHorizontal={true}
                    dragEasing={(v) => v / 2.5}
                    dragStartCallback={this.dragStartBound}
                    dragEndCallback={this.dragEndBound}
                />
            </div>);
    }
}
