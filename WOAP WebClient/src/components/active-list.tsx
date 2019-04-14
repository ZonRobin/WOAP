import * as React from "react";
import Scrollable from "./scrollable";
import {
    DragDropContext,
    Draggable,
    DragHandleProps,
    DragStart,
    Droppable,
    DroppableProvided,
    DropResult
} from "react-beautiful-dnd";
import {ArrayExtended} from "../array-extended";

export interface ActiveListProps {
    listId: string;
    listItemType: string;
    itemsFactory: () => Array<[string, any]>;
    itemDropped: (id: string, sourceDroppable: string, sourceIndex: number, destinationDroppable: string, destinationIndex: number) => void;
    itemDragCanceled?: (id: string, sourceDroppable: string, sourceIndex: number) => void;
    className?: string;
    isHorizontal?: boolean;
    containerRef?: (HTMLElement) => void;
}

export interface ActiveListState {
}

export interface ActiveListDragListener {
    activeListDragStarted();

    activeListDragEnded();
}

export default class ActiveList extends React.Component<ActiveListProps, ActiveListState> {
    static defaultProps = {
        listId: "",
        listItemType: "DEFAULT",
        itemsFactory: () => {
            return [];
        },
        itemDropped: () => {
        },
        itemDragCanceled: () => {

        },
        className: "",
        isHorizontal: false,
        containerRef: () => {
        }
    };
    protected static draggedComponent: { id: string, type: string } = null;
    protected static listBeingDragged: boolean = false;
    protected static dragListeners: ArrayExtended<ActiveListDragListener> = new ArrayExtended();

    protected dragEndBound;
    protected dragStartBound;
    protected refreshAfterDragEnd: boolean = false;
    protected thisListBeingDragged: boolean = false;

    public constructor(props) {
        super(props);
        this.dragEndBound = this.dragEnd.bind(this);
        this.dragStartBound = this.dragStart.bind(this);
    }

    public static isListBeingDragged(): boolean {
        return ActiveList.listBeingDragged;
    }

    public static getDraggedComponentInfo(): { id: string, type: string } {
        return ActiveList.draggedComponent;
    }

    public static addDragListener(listener: ActiveListDragListener) {
        ActiveList.dragListeners.addUnique(listener);
    }

    public static removeDragListener(listener: ActiveListDragListener) {
        ActiveList.dragListeners.remove(listener);
    }

    public getListContainerStyle(): {} {
        if (this.props.isHorizontal) {
            return {
                display: "flex",
                height: "100%",
                flexDirection: "row",
                whiteSpace: "nowrap",
                position: "absolute"
            };
        } else {
            return {
                display: "flex",
                width: "100%",
                flexDirection: "column",
                whiteSpace: "nowrap",
                position: "absolute"
            };
        }
    }

    public dragEnd(result: DropResult) {
        ActiveList.draggedComponent = null;
        ActiveList.listBeingDragged = false;
        this.thisListBeingDragged = false;

        if (result.destination === null) {
            this.props.itemDragCanceled(result.draggableId, result.source.droppableId, result.source.index);
            if (this.refreshAfterDragEnd) {
                this.refreshAfterDragEnd = false;
                this.forceUpdate();
            }
            ActiveList.dragListeners.forEach((listener) => listener.activeListDragEnded());
            return;
        }

        if (result.source.index === result.destination.index) {
            this.props.itemDragCanceled(result.draggableId, result.source.droppableId, result.source.index);
            if (this.refreshAfterDragEnd) {
                this.refreshAfterDragEnd = false;
                this.forceUpdate();
            }
            ActiveList.dragListeners.forEach((listener) => listener.activeListDragEnded());
            return;
        }

        this.refreshAfterDragEnd = false;
        this.props.itemDropped(result.draggableId, result.source.droppableId, result.source.index, result.destination.droppableId, result.destination.index);
        ActiveList.dragListeners.forEach((listener) => listener.activeListDragEnded());
    }

    public dragStart(info: DragStart) {
        ActiveList.listBeingDragged = true;
        this.thisListBeingDragged = true;
        ActiveList.draggedComponent = {id: info.draggableId, type: info.type};
        ActiveList.dragListeners.forEach((listener) => listener.activeListDragStarted());
    }

    public shouldComponentUpdate() {
        if (this.thisListBeingDragged) {
            this.refreshAfterDragEnd = true;
            return false;
        }

        return true;
    }

    public render() {
        return (
            <Scrollable className={this.props.className} isHorizontal={this.props.isHorizontal}>
                <DragDropContext onDragEnd={this.dragEndBound}
                                 onDragStart={this.dragStartBound}>
                    <Droppable droppableId={this.props.listId}
                               direction={this.props.isHorizontal ? "horizontal" : "vertical"}
                               type={this.props.listItemType}
                    >

                        {(droppableProvided, droppableSnapshot) => (
                            <div className="listContainer"
                                 ref={(el) => {
                                     droppableProvided.innerRef(el);
                                     this.props.containerRef(el);
                                 }}
                                 style={this.getListContainerStyle()}
                            >
                                {this.props.itemsFactory().map((item, index) => (
                                    <Draggable key={item[0]}
                                               draggableId={item[0]}
                                               index={index}
                                    >
                                        {(draggableProvided, draggableSnapshot) => (
                                            <div className="draggableItemOuterWrapper">
                                                <div {...draggableProvided.draggableProps}
                                                     ref={draggableProvided.innerRef}
                                                     className={draggableSnapshot.isDragging ? "dragged draggableItemWrapper" : "draggableItemWrapper"}
                                                >
                                                    {(this.props.children as any)(item[1], draggableProvided.dragHandleProps)}
                                                </div>
                                                {draggableProvided.placeholder}
                                            </div>
                                        )}
                                    </Draggable>
                                ))}

                                {droppableProvided.placeholder}
                            </div>
                        )}

                    </Droppable>
                </DragDropContext>
            </Scrollable>
        );
    }
}