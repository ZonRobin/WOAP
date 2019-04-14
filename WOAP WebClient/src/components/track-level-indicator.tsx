import * as React from "react";
import {default as TrackModel, TrackModelControlsListener, TrackModelSignalLevelListener} from "../models/track-model";
import LevelIndicator from "./control-components/level-indicator";
import {Utility} from "../utility";

interface TrackLevelIndicatorProps {
    model: TrackModel;
}

interface TrackLevelIndicatorState {
    dbLevels: [number, number];
    isClipping: boolean;
    isSoloed: boolean;
}

export default class TrackLevelIndicator extends React.Component<TrackLevelIndicatorProps, TrackLevelIndicatorState> implements TrackModelSignalLevelListener, TrackModelControlsListener {
    constructor(props) {
        super(props);

        this.state = {
            dbLevels: [-80, -80],
            isClipping: false,
            isSoloed: props.model.getSolo()
        };
    }

    public componentWillMount() {
        this.props.model.addSignalLevelListener(this);
        this.props.model.addControlsListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeSignalLevelListener(this);
        this.props.model.removeControlsListener(this);
    }

    public gainChanged(newValue: number): void {
    }

    public panChanged(newValue: number): void {
    }

    public muteChanged(newState: boolean): void {
    }

    public soloChanged(newState: boolean): void {
        this.setState({isSoloed: newState});
    }

    public signalLevelsChanged(levels: [number, number], isClipping: boolean): void {
        this.setState({
            isClipping: isClipping,
            dbLevels: [
                Utility.round(Utility.gainToDecibels(levels[0], -80), 1),
                Utility.round(Utility.gainToDecibels(levels[1], -80), 1)
            ],
        });
    }

    public shouldComponentUpdate(props: TrackLevelIndicatorProps, state: TrackLevelIndicatorState) {
        return this.state.dbLevels[0] !== state.dbLevels[0]
            || this.state.dbLevels[1] !== state.dbLevels[1]
            || this.state.isClipping !== state.isClipping
            || this.state.isSoloed !== state.isSoloed
            || this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackLevelIndicatorProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeSignalLevelListener(this);
            this.props.model.removeControlsListener(this);
            newProps.model.addSignalLevelListener(this);
            newProps.model.addControlsListener(this);
        }
    }

    public render() {
        return (
            <div className="trackLevelIndicator">
                <LevelIndicator
                    levels={this.state.dbLevels}
                    isClipping={this.state.isClipping}
                    isSoloed={this.state.isSoloed}
                    minLevel={-80}
                    maxLevel={17}
                    skewFactor={1.5}
                />
            </div>);
    }
}
