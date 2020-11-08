#!/usr/bin/env python3

import random
import sys
import math
import networkx as nx
from dataclasses import dataclass, field
from collections import defaultdict


@dataclass
class Node:
    """
    The dataclass for containing each node of the network

    :param int x: x coordinate of the node
    :param int y: y coordinate of the node
    :param int z: z coordinate of the node
    :param int range_radius: The communication range for the node in radius
    :param int id: Unique identifier
    :param int link_budget: Number of simultaneous links that this node can handle at any time
    :param list neighbors: A list of neighboring node ids
    :param list links: A list of currently connected neighboring node ids. len(node.links) cannot be bigger than link budget
    """

    x: int
    y: int
    z: int
    range_radius: int
    id: int
    link_budget: int
    neighbors: list = field(default_factory=set)
    links: list = field(default_factory=list)


class MeshNetwork:
    """
    Stores the network simulation

    :param int max_x: The x dimension (width) of the virtual simulation space
    :param int max_y: The y dimension (height) of the virtual simulation space
    :param int max_z: The z dimension (depth) of the virtual simulation space
    """

    def __init__(self, x, y, z, min_x=0, min_y=0, min_z=0):
        """
        Constructor
        """

        self.max_x = x
        self.max_y = y
        self.max_z = z
        self.min_x = min_x
        self.min_y = min_y
        self.min_z = min_z

        self.node_coor_x = defaultdict(list)
        self.node_coor_y = defaultdict(list)
        self.node_coor_z = defaultdict(list)

        self.nodes = {}
        self.edges = []

        self.id_counter = 0

    def add_nodes(self, n, max_range, max_link_budget, min_range=1, min_link_budget=1):
        """
        Adds given number of nodes to the simulation space. The communication range of
        each node is choosen randomly from the given communication range. Also, the
        number of simultaneous connections a node can handle is choosen randomly from
        the given link budget range.

        :param int n: Number of nodes to add
        :param int max_range: The maximum communication range for a node
        :param int min_range: The minimum communication range for a node
        :param int max_link_budget: The maximum number of simultaneous links per node
        :param int min_link_budget: The minimum number of simultaneous links per node
        """

        for _ in range(n):
            # Create a new node with random parameters
            curr_node = Node(x=random.randint(self.min_x, self.max_x),
                             y=random.randint(self.min_y, self.max_y),
                             z=random.randint(self.min_z, self.max_z),
                             range_radius=random.randint(min_range, max_range),
                             id=self.next_node_id,
                             link_budget=random.randint(min_link_budget, max_link_budget))

            # Add created node's coordinates to the Network class
            self.node_coor_x[curr_node.x].append(curr_node)
            self.node_coor_y[curr_node.y].append(curr_node)
            self.node_coor_x[curr_node.z].append(curr_node)

            # Add a pointer to the node to the Network class
            self.nodes[curr_node.id] = curr_node

    def find_neighbors(self):
        """
        Iterates over all nodes to mark their neighbors by using the assigned communication
        range values
        """

        # Iterate over the nodes
        for _ in self.nodes:
            # Get the pointer to the current node
            node = self.nodes.get(_)

            # Calculate the maximum distances the node can communicate with
            #   on the coordinate system
            range_max_x = node.x + node.range_radius
            range_min_x = node.x - node.range_radius
            range_max_y = node.y + node.range_radius
            range_min_y = node.y - node.range_radius
            range_max_z = node.z + node.range_radius
            range_min_z = node.z - node.range_radius

            # Use a sliding window to discover nodes that are within the range
            for x in range(range_min_x, range_max_x+1):
                target_node = self.node_coor_x.get(x)

                # If a node is found
                if target_node is not None:
                    # There might be multiple nodes with the same coordinates
                    for n in target_node:
                        # Check each of nodes to see if they are within the range
                        if range_min_y <= n.y <= range_max_y:
                            if range_min_z <= n.z <= range_max_z:
                                # Only add the neighbor if it is not the node itself
                                if n.id != node.id:

                                    # Calculate the distance between the nodes
                                    distance = math.sqrt((node.x - n.x)*(node.x - n.x) +
                                                         (node.y - n.y)*(node.y - n.y) +
                                                         (node.z - n.z)*(node.z - n.z))

                                    # Add as a neighbor if the distance is below
                                    #   the communication range
                                    if distance < node.range_radius:
                                        # Add the neighbor node's id
                                        node.neighbors.add(n.id)

    def create_links(self):
        """
        Iterates over all nodes in a random order to create links/edges between
        the nodes based on the marked neighbors and available link budget. The
        link budget for nodes are decremented whenever a new link is created.
        """

        # Create a randomized node ids to iterate over
        node_ids = list(self.nodes.keys())
        random.shuffle(node_ids)

        # Iterate over the randomized node ids
        for _ in node_ids:

            # Get the pointer to the current node
            node = self.nodes.get(_)

            # Create links if the current node has neighbors and enough link budget
            if len(node.neighbors) > 0 and node.link_budget > 0:

                # Check if link budget exceeds the neighbor count
                if node.link_budget > len(node.neighbors):
                    # Then we can have max neighbor count of links
                    link_count = len(node.neighbors)
                else:
                    # Else, we can use all of our link budget
                    link_count = node.link_budget

                # Choose link_count number of neighbors randomly
                node.links = random.sample(node.neighbors, link_count)

                # Iterate over the selected links/edges
                for node_id in node.links:
                    # Add this link/edge if the neighbor node has enough link budget
                    if self.nodes[node_id].link_budget > 0:
                        # Add this edge to the Network class
                        self.edges.append((node.id, node_id))

                        # Decrease the link budget of the neighbor node
                        self.nodes[node_id].link_budget -= 1

    def remove_all_links(self):
        """
        Removes all links/edges in the network. This attribute can be used to
        create links from scratch without loosing the network node locations.
        """

        # Remove edge information stored in the class
        self.edges = []

        # Remove link/edge information from all node objects
        for _ in self.nodes:
            node = self.nodes[_]
            node.links = []

    def remove_node(self, node_id):
        """
        Removes a given node from the network. This attribute is not working yet.
        """

        # # Get the pointer to the current node
        # node = self.nodes.get(node_id)
        #
        # # Find neighbors of the given node
        # for n in node.neighbors:
        #     # Remove this node from the neighbors' neighbors list
        #     pass
        #
        # # Remove this node's links
        #     # Remove edges from the Network class
        #
        # # Remove this node from the Network class (coordinates and node dict)
        #
        # # Rebuild the network connections <<<<< not sure about this one

    def remove_disconnected_nodes(self):
        """
        Removes all nodes that do not have any neighbors and thus connections
        """

        temp_nodes = self.nodes.copy()
        for _ in temp_nodes:
            node = self.nodes[_]
            if len(node.neighbors) == 0:
                del self.nodes[node.id]

    def update_mst(self):
        """
        Converts the network to a NetworkX Graph object and finds the largest
        minimum spanning tree.
        """

        # Convert to NetworkX graph
        self.nxg = nx.Graph()
        self.nxg.add_nodes_from(self.nodes.keys())
        self.nxg.add_edges_from(self.edges)

        # Find minimum spanning tree
        mst = (self.nxg.subgraph(_) for _ in nx.connected_components(self.nxg))
        mst = list(mst)[0]

        self.mst = mst

    def draw(self, mst=True, node_size=60, width=2):
        """
        Visualizes the created network

        :param bool mst: Should I draw the largest minimum spanning tree?, defaults to True
        :param int node_size: Size of the nodes in the visualization, defaults to 60
        :param int width: The tickness of the links in the visualization, defaults to 2
        """

        # We need matplotlib to draw
        import matplotlib.pyplot as plt

        # If NetworkX graph wasn't created, create it
        try:
            self.nxg
        except AttributeError:
            self.update_mst()

        # Plot the graph
        if mst:
            nx.draw(self.mst, node_size=node_size, width=width)
        else:
            nx.draw(self.nxg, node_size=node_size, width=width)

        plt.show()

    @property
    def mst_edges(self, update=True):
        """
        Returns a dict of edges in the largest minimum spanning tree, with tuples
        of edges as the keys and the distances between the nodes as the values.
        To be more clear, this is the exact format:
        { (node1_id, node2_id) : distance between node1 and node2,
          (node3_id, node4_id) : distance between node3 and node4, ... }

        :param bool update: Should I recalculate the largest minimum spanning tree, defaults to True
        :return: Dictionary of tuples of edges as keys and distances as the values
        :rtype: dict
        """

        edges = {}

        # Get the latest minimum spanning tree
        if update:
            self.update_mst()

        # Get and convert the edge list
        for line in nx.generate_edgelist(self.mst, data=False):
            temp = line.split(' ')

            node0 = self.nodes.get(int(temp[0]))
            node1 = self.nodes.get(int(temp[1]))

            if node0 is not None and node1 is not None:
                # Calculate the distance between the nodes
                distance = math.sqrt((node1.x - node0.x)*(node1.x - node0.x) +
                                     (node1.y - node0.y)*(node1.y - node0.y) +
                                     (node1.z - node0.z)*(node1.z - node0.z))

                # Save the result
                edges[(int(temp[0]), int(temp[1]))] = int(distance)

        return edges

    @property
    def mst_node_count(self):
        """
        Returns the node count in the largest minimum spanning tree

        :return: The node count in the largest minimum spanning tree
        :rtype: int
        """

        return len(self.mst)

    @property
    def next_node_id(self):
        """
        Returns the next node id. This is simply a counter.

        :return: The next node id
        :rtype: int
        """

        self.id_counter += 1
        return self.id_counter
