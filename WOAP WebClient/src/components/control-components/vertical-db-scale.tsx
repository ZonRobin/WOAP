import * as React from "react";
import {Utility} from "../../utility";
import ResizeWatcherListener = Utility.ResizeWatcherListener;

interface VerticalDbScaleState {

}

interface VerticalDbScaleProps {
    minValue?: number;
    maxValue?: number;
    skewFactor?: number;
    width?: number;
    margin?: number;
}

export default class VerticalDbScale extends React.Component<VerticalDbScaleProps, VerticalDbScaleState> implements ResizeWatcherListener {
    static defaultProps = {
        minValue: -100,
        maxValue: 10,
        skewFactor: 1,
        width: 40,
        margin: 0
    };

    protected onRefBound;
    protected rootElement: Element = null;

    public constructor(props) {
        super(props);

        this.onRefBound = this.onRef.bind(this);
    }

    public componentDidMount() {
        window.WOAP.ResizeWatcher.addListener(this);
    }

    public componentWillUnmount() {
        window.WOAP.ResizeWatcher.removeListener(this);
    }

    public documentResized(event, orientationChanged: boolean) {
        this.forceUpdate();
    }

    public shouldComponentUpdate(props: VerticalDbScaleProps, state: VerticalDbScaleState) {
        return this.props.width !== props.width
            || this.props.minValue !== props.minValue
            || this.props.maxValue !== props.maxValue
            || this.props.skewFactor !== props.skewFactor;
    }

    public onRef(e: Element) {
        this.rootElement = e;
        this.forceUpdate();
    }

    public getRootHeight(): number {
        return this.rootElement === null ? 100 : this.rootElement.clientHeight;
    }

    public getLevelsWithText(height: number): number[] {
        if (height > 100)
            return [10, 0, -10, -30, -50, -80];
        else if (height > 65)
            return [10, 0, -10, -30, -80];
        else if (height > 50)
            return [10, 0, -30, -80];
        else
            return [0, -80];
    }

    public getLevelsWithoutText(height: number): number[] {
        if (height > 70)
            return [5, -5, -20, -40, -60, -110];
        else
            return [];
    }

    public render() {
        let width = this.props.width;
        let height = this.getRootHeight() - this.props.margin * 2;
        let margin = this.props.margin;
        let range = this.props.maxValue - this.props.minValue;
        let color = "rgb(30, 30, 30)";

        let nodes = [];
        for (let level of this.getLevelsWithText(height)) {
            let levelRelative = (level - this.props.minValue) / range;
            let levelPosY = height - (height * Math.pow(levelRelative, this.props.skewFactor));

            nodes.push(<rect x="0" y={levelPosY + margin} width={width - 25} height={1} style={{fill: color}}
                             key={level}/>);
            nodes.push(<text textAnchor="end" x={width} y={levelPosY + 4 + margin} fontFamily="Verdana" fontSize="11"
                             key={level + "t"}>{level}</text>);
        }

        for (let level of this.getLevelsWithoutText(height)) {
            let levelRelative = (level - this.props.minValue) / range;
            let levelPosY = height - (height * Math.pow(levelRelative, this.props.skewFactor));

            nodes.push(<rect x="0" y={levelPosY + margin} width={width} height={1} style={{fill: color}} key={level}/>);
        }

        return (
            <div className="verticalDbScale"
                 ref={this.onRefBound}
            >
                <svg width={this.props.width} height={this.getRootHeight()} shapeRendering="crispEdges">
                    {nodes}
                </svg>
            </div>);
    }
}