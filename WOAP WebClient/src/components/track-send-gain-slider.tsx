import * as React from "react";
import LinearSlider from "./control-components/linear-slider";
import VerticalDbScale from "./control-components/vertical-db-scale";
import TrackSendModel, {TrackSendModelListener} from "../models/track-send-model";
import Color from "../helpers/color/color";

interface TrackSendGainSliderProps {
    model: TrackSendModel;
}

interface TrackSendGainSliderState {
    gainLevel: number;
}

export default class TrackSendGainSlider extends React.Component<TrackSendGainSliderProps, TrackSendGainSliderState> implements TrackSendModelListener {
    static defaultProps = {
        model: null,
        height: 200
    };
    protected isBeingDragged: boolean = false;

    protected dragStartBound;
    protected dragEndBound;
    protected valueChangedCallbackBound;

    constructor(props) {
        super(props);

        this.state = {
            gainLevel: this.props.model.getGain()
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
        if (!this.isBeingDragged)
            this.setState({gainLevel: newValue});
    }

    public panChanged(newValue: number): void {
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
        this.props.model.setGain(newValue);
        this.setState({gainLevel: newValue});
    }

    public shouldComponentUpdate(props: TrackSendGainSliderProps, state: TrackSendGainSliderState) {
        return this.state.gainLevel !== state.gainLevel
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackSendGainSliderProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            newProps.model.addListener(this);
            this.setState({gainLevel: newProps.model.getGain()});
        }
    }

    public dragStart() {
        this.isBeingDragged = true;
    }

    public dragEnd() {
        this.isBeingDragged = false;
        this.setState({gainLevel: this.props.model.getGain()});
    }

    protected thumb = <svg width="30" height="40" shapeRendering="crispEdges">
        <rect x="0" y="0" width="100%" height="100%" rx="3" ry="3"
              style={{fill: "rgb(40, 40, 40)"}}/>

        <rect x="5%" y="50%" width="90%" height="2" style={{fill: "rgb(230, 230, 230)"}}/>

        <rect x="5%" y="35%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>
        <rect x="5%" y="65%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>

        <rect x="5%" y="20%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>
        <rect x="5%" y="80%" width="90%" height="1" style={{fill: "rgb(100, 100, 100)"}}/>
    </svg>;

    public render() {
        return (
            <div className="gainSlider trackSendGainSlider">
                <VerticalDbScale width={30}
                                 margin={20}
                                 minValue={-145}
                                 maxValue={10}
                                 skewFactor={3}
                />
                <LinearSlider
                    value={this.state.gainLevel}
                    minValue={-145}
                    maxValue={10}
                    skewFactor={3}
                    valueChangedCallback={this.valueChangedCallbackBound}
                    dragStartCallback={this.dragStartBound}
                    dragEndCallback={this.dragEndBound}
                >
                    {this.thumb}
                </LinearSlider>
            </div>);
    }
}
