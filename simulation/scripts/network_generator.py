#!/usr/bin/env python

from pprint import pprint as print
import random
import sys
import math
import networkx as nx
from dataclasses import dataclass, field
from collections import defaultdict
from pandas import DataFrame, unique
from sklearn.cluster import KMeans


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
    :param list neighbors: A list of neighboring node ids, defaults to []
    :param list links: A list of currently connected neighboring node ids, defaults to []
    :param bool hub: Determine whether the node is a hub or not, defaults to False
    :param int hub_id: The hub if the node is a hub
    """

    x: int
    y: int
    z: int
    range_radius: int
    id: int
    link_budget: int
    neighbors: list = field(default_factory=set)
    links: list = field(default_factory=list)
    hub: bool = False
    hub_id: int = 0


class Network():
    """
    Stores the network simulation. Originally designed to contain a 3D space but
    please don't use the z axis for now.

    :param int max_x: The x dimension (width) of the virtual simulation space
    :param int max_y: The y dimension (height) of the virtual simulation space
    :param int max_z: The z dimension (depth) of the virtual simulation space
    """

    def __init__(self, x, y, z=0, min_x=0, min_y=0, min_z=0):
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
            self.node_coor_z[curr_node.z].append(curr_node)

            # Add a pointer to the node to the Network class
            self.nodes[curr_node.id] = curr_node

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

    @property
    def next_node_id(self):
        """
        Returns the next node id. This is simply a counter.

        :return: The next node id
        :rtype: int
        """

        self.id_counter += 1
        return self.id_counter


class Mesh(Network):
    """
    Extends Network class to implement a mesh network
    """

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

    @property
    def network_edges(self):
        """
        Returns a dict of edges in the largest minimum spanning tree, with tuples
        of edges as the keys and the distances between the nodes as the values.
        To be more clear, this is the exact format:
        { (node1_id, node2_id) : distance between node1 and node2,
          (node3_id, node4_id) : distance between node3 and node4, ... }

        :return: Dictionary of tuples of edges as keys and distances as the values
        :rtype: dict
        """

        edges = {}

        # Get the latest minimum spanning tree
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
    def network_node_count(self):
        """
        Returns the node count in the largest minimum spanning tree

        :return: The node count in the largest minimum spanning tree
        :rtype: int
        """

        return len(self.mst)

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

    @property
    def node_list(self):
        """
        Returns the list of node connected nodes.

        :return: List of connected nodes
        :rtype: list
        """
        return self.mst.nodes()

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


class Star(Network):
    """
    Extends Network class to implement a star network
    """

    def __init__(self, *args, **kwargs):
        """
        Constructor
        """

        super(Star, self).__init__(*args, **kwargs)
        self.hub_budget = 0
        self.number_of_hubs_used = 0
        self.centroids_list = []
        self.hubs = {}

    def set_hub_constraints(self, n, max_range, max_link_budget, min_range=1, min_link_budget=1):
        """
        Set the constraints for the hubs and specifiy the maximum number of hubs that can be used.
        This method doesn't actually place any hubs, it just defines the constraints. Also, same
        constraints are used for all hubs, as opposed to the nodes, where each node has a randomized
        constraint value within the given range.

        :param int n: The maximum number of hubs to add to the network
        :param int max_range: The maximum communication range for a hub
        :param int min_range: The minimum communication range for a hub
        :param int max_link_budget: The maximum number of simultaneous links per hub
        :param int min_link_budget: The minimum number of simultaneous links per hub
        """

        # All hubs have to have the same range as opposed to the nodes
        hub_range = random.randint(min_range, max_range)
        hub_link_budget = random.randint(min_link_budget, max_link_budget)

        self.hub_budget += n
        self.hub_range = hub_range
        self.hub_link_budget = hub_link_budget

    def find_neighbors(self, percent_network_coverage, draw=False):
        """
        Uses kmeans method to create clusters on the network and assigns hubs to
        these clusters based on the provided constraints. The percent_network_coverage
        is used in conjunction with the hub constraints. You can leave out a determined
        percentage of the network using percent_network_coverage argument. This is useful,
        because covering every single possible node might not be possible and realistic
        considering the fact that the nodes were distributed randomly.

        This method needs to be heavily refactored!

        :param int percent_network_coverage: The percentage of the nodes that should be covered
        :param bool draw: Should I visualize the clusters?, defaults to False
        :return: Number of hubs/clusters formed as a result of the kmeans algorithm
        """

        hub_budget = self.hub_budget

        # There has to be nodes to find neighborhoods
        if hub_budget == 0:
            raise Exception("HubBudgetIsNotSet")

        # Fill the required data
        data = {'x': [], 'y': [], 'node_id': []}
        for _ in self.nodes:
            node = self.nodes[_]
            data['x'].append(node.x)
            data['y'].append(node.y)
            data['node_id'].append(node.id)

        # Start with only one hub and check if the network can work with it
        # Keep increasing the hub count until the constraints are satisfied or the
        #   hub_budget is reached.
        constraints_are_satisfied = False
        for i in range(1, hub_budget):
            # Transform node list into pandas dataframe
            # A fresh dataframe is needed for each iteration
            df = DataFrame(data, columns=['x', 'y'])

            # Fit the data in to N clusters/hubs
            kmeans = KMeans(n_clusters=i).fit(df)
            df['cluster_label'] = kmeans.labels_
            centroids_list = kmeans.cluster_centers_

            df['centroid_x'] = ''
            df['centroid_y'] = ''
            df['distance_to_centroid'] = ''
            df['node_id'] = 0

            # Calculate the distance between the nodes and hubs
            nodes_within_hub_range = 0
            for idx, row in df.iterrows():
                centroid_x = centroids_list[int(df.at[idx, 'cluster_label'])][0]
                centroid_y = centroids_list[int(df.at[idx, 'cluster_label'])][1]
                x, y = df.at[idx, 'x'], df.at[idx, 'y']

                # Calculate the distance between the node and the closest hub
                distance_to_centroid = math.sqrt((centroid_x - x)*(centroid_x - x) +
                                                 (centroid_y - y)*(centroid_y - y))

                df.at[idx, 'centroid_x'] = centroid_x
                df.at[idx, 'centroid_y'] = centroid_y
                df.at[idx, 'distance_to_centroid'] = distance_to_centroid

                # Check if this node is within the range of a hub
                if distance_to_centroid < self.hub_range:
                    nodes_within_hub_range += 1

            # If the percentage of nodes that are out of the reach of the hub is smaller than distance_treshold,
            #   stop trying and increasing the hub count
            if ((nodes_within_hub_range / len(df)) * 100) > percent_network_coverage:
                constraints_are_satisfied = True
                self.centroids_list = centroids_list
                # Append node_ids to dataframe
                df['node_id'] = data['node_id']

                # Stop iterating since we satisfied the constraints
                break

        # Raise an exception if given constraints cannot be satisfied
        if not constraints_are_satisfied:
            raise Exception("StarNetworkConstraintsCanNotBeSatisfied")

        # Save the least number of hubs required
        self.number_of_hubs_used = len(unique(df['cluster_label']))

        # Draw the resulting clusters if requested
        if draw:
            import matplotlib.pyplot as plt

            # Plot the clusters
            plt.scatter(df['x'], df['y'], c=kmeans.labels_.astype(float), s=50, alpha=0.5)
            plt.scatter(centroids_list[:, 0], centroids_list[:, 1], c='red', s=50)

            fig = plt.gcf()
            ax = fig.gca()

            # Mark the ranges of the hubs
            for c in centroids_list:
                circle = plt.Circle((c[0], c[1]), radius=self.hub_range, color='r', fill=False)
                ax.add_artist(circle)

            plt.show()

        # Place required hubs
        self.place_hubs()

        # Determine the neighbors of each hub using the created dataframe
        for _ in self.hubs:
            # Get the pointer to the current node
            node = self.hubs.get(_)

            for idx, row in df.iterrows():
                if row['cluster_label'] == node.hub_id:
                    # Add node's id to hub
                    node.neighbors.add(row['node_id'])

                    # Add hub's id to node
                    self.nodes.get(row['node_id']).neighbors.add(node.id)

        return self.number_of_hubs_used

    def place_hubs(self):
        """
        Place a hub to each neighborhood that was determined through the kmeans
        algorithm
        """

        # Use the centroid list to place the hubs
        for idx, c in enumerate(self.centroids_list):
            # Create a new node with random parameters
            curr_node = Node(x=c[0],
                             y=c[1],
                             z=0,
                             range_radius=self.hub_range,
                             id=self.next_node_id,
                             link_budget=self.hub_link_budget,
                             hub=True,
                             hub_id=idx)

            # Add created node's coordinates to the Network class
            self.node_coor_x[curr_node.x].append(curr_node)
            self.node_coor_y[curr_node.y].append(curr_node)
            self.node_coor_x[curr_node.z].append(curr_node)

            # Add a pointer to the node to the Network class
            self.nodes[curr_node.id] = curr_node
            self.hubs[curr_node.id] = curr_node

    def create_links(self, *args, **kwargs):
        """
        Extend the base create_links method to additionally connect hubs to hubs.
        Input arguments are same as the original create_links method.
        """

        super(Star, self).create_links(*args, **kwargs)

        # Iterate over the hubs
        prev_node_id = None
        for _ in self.hubs:
            # Get the pointer to the current node
            node = self.hubs.get(_)

            # Create a connection if hub has neighbor nodes
            if (prev_node_id is not None) and (len(node.neighbors) != 0):
                self.edges.append((node.id, prev_node_id))

            prev_node_id = node.id

    def remove_disconnected_nodes(self, *args, **kwargs):
        """
        Extend the base remove_disconnected_nodes method to remove unused hubs
        as well. Input arguments are same as the original remove_disconnected_nodes
        method.
        """

        super(Star, self).remove_disconnected_nodes(*args, **kwargs)

        temp_hubs = self.hubs.copy()
        for _ in temp_hubs:
            hub = self.hubs[_]
            if len(hub.neighbors) == 0:
                del self.hubs[hub.id]

    @property
    def network_edges(self):
        """
        Returns a dict of edges in the network, with tuples of edges as the keys
        and the distances between the nodes as the values.
        To be more clear, this is the exact format:
        { (node1_id, node2_id) : distance between node1 and node2,
          (node3_id, node4_id) : distance between node3 and node4, ... }

        :return: Dictionary of tuples of edges as keys and distances as the values
        :rtype: dict
        """

        edges = {}

        # Convert to NetworkX graph
        self.nxg = nx.Graph()
        self.nxg.add_nodes_from(self.nodes.keys())
        self.nxg.add_edges_from(self.edges)

        # Get and convert the edge list
        for line in nx.generate_edgelist(self.nxg, data=False):
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
    def network_node_count(self):
        """
        Returns the node count in the network

        :return: The node count in the network
        :rtype: int
        """

        return len(self.nxg)

    @property
    def network_hubs(self):
        """
        Returns the list of ids of hubs in the network

        :return: The the list of ids of hubs in the network
        :rtype: list
        """

        return list(self.hubs.keys())

    @property
    def node_list(self):
        """
        Returns the list of node connected nodes.

        :return: List of connected nodes
        :rtype: list
        """
        return self.nxg.nodes()

    def draw(self, node_size=60, width=2):
        """
        Visualizes the created network

        :param int node_size: Size of the nodes in the visualization, defaults to 60
        :param int width: The tickness of the links in the visualization, defaults to 2
        """

        # We need matplotlib to draw
        import matplotlib.pyplot as plt

        # If NetworkX graph wasn't created, create it
        try:
            self.nxg
        except AttributeError:
            # Convert to NetworkX graph
            self.nxg = nx.Graph()
            self.nxg.add_nodes_from(self.nodes.keys())
            self.nxg.add_edges_from(self.edges)

        # Plot the graph
        nx.draw(self.nxg, node_size=node_size, width=width)

        plt.show()
