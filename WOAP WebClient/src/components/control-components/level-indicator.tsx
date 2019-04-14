import * as React from "react";
import {Utility} from "../../utility";

interface LevelIndicatorState {

}

interface LevelIndicatorProps {
    levels: [number, number];
    isClipping: boolean;
    isSoloed: boolean;
    minLevel?: number;
    maxLevel?: number;
    skewFactor?: number;
}

export default class LevelIndicator extends React.Component<LevelIndicatorProps, LevelIndicatorState> {
    static defaultProps = {
        levels: [-100, -100],
        isClipping: false,
        isSoloed: false,
        minLevel: -100,
        maxLevel: 0,
        skewFactor: 1
    };

    protected levelToNormalizedLength(level: number): number {
        return Utility.inRange(0, 1, Math.pow((level - this.props.minLevel) / (this.props.maxLevel - this.props.minLevel), this.props.skewFactor));
    }

    public render() {
        let leftHeight = Math.floor(this.levelToNormalizedLength(this.props.levels[0]) * 100);
        let rightHeight = Math.floor(this.levelToNormalizedLength(this.props.levels[1]) * 100);

        return (
            <div className={"levelIndicator" + (this.props.isClipping ? " clipping" : "") + (this.props.isSoloed ? " soloed" : "")}>
                <div className="levelIndicatorBar" style={{height: leftHeight + "%"}}/>
                <div className="levelIndicatorBar" style={{height: rightHeight + "%"}}/>
            </div>);
    }
}