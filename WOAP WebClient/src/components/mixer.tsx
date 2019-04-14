import * as React from "react";
import {Utility, UUID} from "../utility";
import Track from "./track";
import {default as TrackStore, TrackStoreListener} from "../stores/track-store";
import TrackModel from "../models/track-model";
import ActiveList, {ActiveListDragListener} from "./active-list";
import TrackDetail from "./track-detail";
import {faPlus, faTrashAlt} from "@fortawesome/fontawesome-free-solid";
import IconButton from "./icon-button";
import {ContextMenu, ContextMenuProvider} from "react-contexify";
import ContextMenuContent from "./context-menu-content";
import ButtonList from "./button-list";
import {
    DragDropContext,
    Draggable,
    DragHandleProps,
    Droppable,
    DroppableProvided,
    DropResult
} from "react-beautiful-dnd";
import MouseWatcherListener = Utility.MouseWatcherListener;

interface MixerProps {
}

interface MixerState {
}

export default class Mixer extends React.Component<MixerProps, MixerState> implements TrackStoreListener {
    protected trackStore: TrackStore;
    protected onClickBound;
    protected contentElement: HTMLElement;

    public constructor(props) {
        super(props);

        this.onClickBound = this.onClick.bind(this);
    }

    public componentWillMount() {
        this.trackStore = window.WOAP.ServiceLocator.getStoresManager().TrackStore;
        this.trackStore.addListener(this);
    }

    public componentWillUnmount(): void {
        window.WOAP.ServiceLocator.getStoresManager().TrackStore.removeListener(this);
    }

    public trackAdded(track: TrackModel, position: number): void {
        this.forceUpdate();
    }

    public trackRemoved(track: TrackModel): void {
        this.forceUpdate();
    }

    public trackMoved(trackId: UUID, oldPosition: number, newPosition: number): void {
        this.forceUpdate();
    }

    public trackSelected(track: TrackModel): void {
        this.forceUpdate();
    }

    public shouldComponentUpdate(props: MixerProps, state: MixerState) {
        return false;
    }

    public onClick(ev) {
        if (ev.target === this.contentElement
            || (typeof this.contentElement.children[0] !== "undefined" && ev.target === this.contentElement.children[0].children[0])
        ) {
            if (!window.WOAP.ContextMenuManager.isContextMenuOpened()
                && !window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown())
                this.trackStore.selectTrack(null);
        }
    }

    public render() {
        let selectedTrack = this.trackStore.getSelectedTrack();

        return (
            <div className="mixer">
                <MixerBottomBar/>
                <div className="mixerContent"
                     onClick={this.onClickBound}
                     ref={(e) => this.contentElement = e}
                >
                    <ActiveList
                        listId="trackList"
                        listItemType="track"
                        className="trackList"
                        isHorizontal={true}
                        itemsFactory={() =>
                            this.trackStore.getTracks().map((item: TrackModel) => [item.getId(), item]) as [string, any]
                        }
                        itemDropped={(trackId, s, src, d, destinationIndex) => {
                            this.trackStore.moveTrack(trackId, destinationIndex);
                        }}
                    >
                        {(model, dragHandleProps) => <Track model={model} dragHandleProps={dragHandleProps}
                                                            selected={model === selectedTrack}/>}
                    </ActiveList>
                    {selectedTrack !== null ? <TrackDetail model={selectedTrack}/> : []}
                </div>
            </div>
        );
    }
}

interface MixerBottomBarState {
    trashButtonActive: boolean;
    trashButtonEnabled: boolean;
}

export class MixerBottomBar extends React.Component<{}, MixerBottomBarState> implements ActiveListDragListener, MouseWatcherListener {
    protected barClickBound;
    protected barElement: Element;
    protected trashButtonMouseUpBound;
    protected trashButtonMouseEnterBound;
    protected trashButtonMouseLeaveBound;
    protected trashButtonElement: Element;
    protected trashButtonPosition;

    public constructor(props) {
        super(props);

        this.state = {
            trashButtonActive: false,
            trashButtonEnabled: false
        };

        this.barClickBound = this.barClick.bind(this);
        this.trashButtonMouseUpBound = this.trashButtonMouseUp.bind(this);
        this.trashButtonMouseEnterBound = this.trashButtonMouseEnter.bind(this);
        this.trashButtonMouseLeaveBound = this.trashButtonMouseLeave.bind(this);

        ActiveList.addDragListener(this);
    }

    public componentWillUnmount() {
        ActiveList.removeDragListener(this);
        window.WOAP.MouseWatcher.removeListener(this);
    }

    public shouldComponentUpdate(props, state) {
        return this.state.trashButtonActive !== state.trashButtonActive
            || this.state.trashButtonEnabled !== state.trashButtonEnabled;
    }

    public activeListDragStarted() {
        this.setState({trashButtonEnabled: true});

        window.WOAP.MouseWatcher.addListener(this);
        this.trashButtonPosition = this.trashButtonElement.getBoundingClientRect();
    }

    public activeListDragEnded() {
        this.setState({trashButtonEnabled: false});
        window.WOAP.MouseWatcher.removeListener(this);
    }

    public dragStarted(position: Utility.Position): void {
    }

    public dragMove(position: Utility.Position): void {
        if (Utility.isInRange(this.trashButtonPosition.left, this.trashButtonPosition.right, position.x)
            && Utility.isInRange(this.trashButtonPosition.top, this.trashButtonPosition.bottom, position.y)) {
            if (!this.state.trashButtonActive) {
                this.setState({trashButtonActive: true});
            }
        } else {
            if (this.state.trashButtonActive) {
                this.setState({trashButtonActive: false});
            }
        }
    }

    public dragEnded(position: Utility.Position): void {
        if (Utility.isInRange(this.trashButtonPosition.left, this.trashButtonPosition.right, position.x)
            && Utility.isInRange(this.trashButtonPosition.top, this.trashButtonPosition.bottom, position.y)
            && this.state.trashButtonActive
        ) {
            this.trashNode();
            this.setState({trashButtonActive: false});
        }
    }

    public trashButtonMouseUp() {
        if (!ActiveList.isListBeingDragged()
            || !window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown()
            || !this.state.trashButtonActive
        ) {
            return;
        }

        this.trashNode();
        this.setState({trashButtonActive: false});
    }

    public trashButtonMouseEnter() {
        if (ActiveList.isListBeingDragged()
            && window.WOAP.MouseWatcher.isMouseDown()
            && window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown()
        ) {
            this.setState({trashButtonActive: true});
        } else if (this.state.trashButtonEnabled) {
            this.setState({trashButtonEnabled: false});
        }
    }

    public trashButtonMouseLeave() {
        this.setState({trashButtonActive: false});
    }

    public barClick(ev): void {
        if (ev.target === this.barElement) {
            if (!window.WOAP.ContextMenuManager.isContextMenuOpened()
                && !window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown())
                window.WOAP.ServiceLocator.getStoresManager().TrackStore.selectTrack(null);
        }
    }

    public render() {
        return (
            <div className="bottomBar"
                 onClick={this.barClickBound}
                 ref={(e) => this.barElement = e}
            >
                <ContextMenuProvider id="addTrackMenu" event="onClick">
                    <IconButton faIcon={faPlus} className="addTrackButton"/>
                </ContextMenuProvider>
                <ContextMenu id="addTrackMenu">
                    <AddTrackList/>
                </ContextMenu>

                <div onMouseUp={this.trashButtonMouseUpBound}
                     onMouseEnter={this.trashButtonMouseEnterBound}
                     onMouseLeave={this.trashButtonMouseLeaveBound}
                     ref={(e) => this.trashButtonElement = e}
                >
                    <IconButton
                        faIcon={faTrashAlt}
                        className={"trashButton" + (this.state.trashButtonEnabled ? " enabled" : "")}
                        active={this.state.trashButtonActive}
                    />
                </div>
            </div>
        );
    }

    protected trashNode() {
        let draggedComponent = ActiveList.getDraggedComponentInfo();
        if (draggedComponent.type === "track") {
            window.WOAP.ServiceLocator.getStoresManager().TrackStore.removeTrack(draggedComponent.id);
        } else if (draggedComponent.type === "plugin") {
            let ids = draggedComponent.id.split(",");
            if (ids.length === 2) {
                window.WOAP.ServiceLocator.getStoresManager().TrackStore.getTrackById(ids[0]).removePlugin(ids[1]);
            }
        } else if (draggedComponent.type === "send") {
            let ids = draggedComponent.id.split(",");
            if (ids.length === 2) {
                window.WOAP.ServiceLocator.getStoresManager().TrackStore.getTrackById(ids[0]).removeSend(ids[1]);
            }
        }
    }
}

export class AddTrackList extends React.Component<{}, {}> {
    public render() {
        return (
            <ContextMenuContent className="addTrackButtonList">
                <ButtonList buttons={[
                    [TrackModel.Type.AudioTrack, "Add audio track"],
                    [TrackModel.Type.FxTrack, "Add Fx track"],
                    [TrackModel.Type.MixBusTrack, "Add mix bus track"],
                    [TrackModel.Type.OutputBusTrack, "Add output bus track"],
                ]} onItemClick={(trackType: number) => {
                    window.WOAP.ServiceLocator.getStoresManager().TrackStore.addTrack(trackType);
                    window.WOAP.ContextMenuManager.dismissMenu();
                }}/>
            </ContextMenuContent>
        );
    }
}