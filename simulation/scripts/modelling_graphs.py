#!/usr/bin/env python


import matplotlib.pyplot as plt
from network_generator import Mesh, Star

vspace_x = 2000
vspace_y = 2000
node_count = 50

node_max_range = 350
node_min_range = 300
node_max_link_budget = 8
node_min_link_budget = 4

hub_max_range = 250
hub_min_range = 200
hub_max_link_budget = 250
hub_min_link_budget = 200
star_percent_network_coverage = 95

draw_edges = True
draw_node_range = True

draw_hubs = True
draw_hub_range = True


def main():

    draw_mesh()

    # update setting for the star network
    global node_count, draw_node_range
    node_count = 300
    draw_node_range = False
    draw_star()


def draw_mesh():
    """
    MESH
    """
    # Create the network
    mesh = Mesh(x=vspace_x, y=vspace_y, z=0)
    mesh.add_nodes(n=node_count,
                   max_range=node_max_range,
                   min_range=node_min_range,
                   max_link_budget=node_max_link_budget,
                   min_link_budget=node_min_link_budget)
    mesh.find_neighbors()
    mesh.create_links()
    mesh.remove_disconnected_nodes()
    mesh.network_edges

    fig, ax = plt.subplots()

    # Get the coordinates
    coor = {'x': [], 'y': []}
    for node in mesh.node_list:
        coor['x'].append(mesh.nodes[node].x)
        coor['y'].append(mesh.nodes[node].y)
        # Place radius circles
        if draw_node_range:
            circle = plt.Circle((mesh.nodes[node].x, mesh.nodes[node].y),
                                radius=mesh.nodes[node].range_radius,
                                color='r', fill=False, linestyle='--')
            ax.add_artist(circle)

    # Place the nodes
    ax.scatter(coor['x'], coor['y'], color='b', s=30, label='Node')

    # Create black lines between nodes
    if draw_edges:
        for edge in mesh.network_edges:
            x0 = mesh.nodes[edge[0]].x
            x1 = mesh.nodes[edge[1]].x
            y0 = mesh.nodes[edge[0]].y
            y1 = mesh.nodes[edge[1]].y
            plt.plot([x0, x1], [y0, y1], 'k-', zorder=0)

    # Needed for labelling
    if draw_edges:
        plt.plot([0, 0], [0, 0], 'k-', label='Connections between nodes')
    if draw_node_range:
        plt.plot([0, 0], [0, 0], 'r--', label='Node communication range')

    # Axes limits & legend
    plt.xlim(0, vspace_x)
    plt.ylim(0, vspace_y)
    plt.legend(loc="upper right", facecolor='white', framealpha=1)

    plt.savefig('mesh.png', dpi=300)
    # plt.show()


def draw_star():
    """
    STAR
    """
    # Create the network
    mesh = Mesh(x=vspace_x, y=vspace_y, z=0)
    mesh.add_nodes(n=node_count,
                   max_range=node_max_range,
                   min_range=node_min_range,
                   max_link_budget=node_max_link_budget,
                   min_link_budget=node_min_link_budget)

    star = Star(x=vspace_x, y=vspace_y, z=0)
    # Nodes have to have a link budget of 1 in the star network
    star.add_nodes(n=node_count,
                   max_range=node_max_range,
                   min_range=node_min_range,
                   max_link_budget=1,
                   min_link_budget=1)
    star.set_hub_constraints(n=node_count,
                             max_range=hub_max_range,
                             min_range=hub_min_range,
                             max_link_budget=hub_max_link_budget,
                             min_link_budget=hub_min_link_budget)
    star.find_neighbors(percent_network_coverage=star_percent_network_coverage, draw=False)
    star.place_hubs()
    star.create_links()
    star.remove_disconnected_nodes()
    star.network_edges
    star.node_list
    hubs = star.network_hubs

    fig, ax = plt.subplots()

    # Get the coordinates of regular nodes
    coor = {'x': [], 'y': []}
    for node in star.node_list:
        if node not in hubs:
            coor['x'].append(star.nodes[node].x)
            coor['y'].append(star.nodes[node].y)
            # Place radius circles
            if draw_node_range:
                circle = plt.Circle((star.nodes[node].x, star.nodes[node].y),
                                    radius=star.nodes[node].range_radius,
                                    color='r', fill=False, linestyle='--')
                ax.add_artist(circle)

    # Place the nodes
    ax.scatter(coor['x'], coor['y'], color='b', s=30, label='Node')

    # Get the coordinates of hubs
    if draw_hubs:
        hub_coor = {'x': [], 'y': []}
        for hub in hubs:
            hub_coor['x'].append(star.nodes[hub].x)
            hub_coor['y'].append(star.nodes[hub].y)
            # Place radius circles
            if draw_hub_range:
                circle = plt.Circle((star.nodes[hub].x, star.nodes[hub].y),
                                    radius=star.nodes[hub].range_radius,
                                    color='g', fill=False, linestyle='--')
                ax.add_artist(circle)

        # Place the hubs
        ax.scatter(hub_coor['x'], hub_coor['y'], color='g', s=60, label='Hub')

    # Create black lines between nodes
    if draw_edges:
        for edge in star.network_edges:
            if (edge[0] not in hubs) or (edge[1] not in hubs):
                x0 = star.nodes[edge[0]].x
                x1 = star.nodes[edge[1]].x
                y0 = star.nodes[edge[0]].y
                y1 = star.nodes[edge[1]].y
                plt.plot([x0, x1], [y0, y1], 'k-', zorder=0)

    # Needed for labelling
    if draw_edges:
        plt.plot([0, 0], [0, 0], 'k-', label='Connections between nodes')
    if draw_node_range:
        plt.plot([0, 0], [0, 0], 'r--', label='Node communication range')
    if draw_hub_range:
        plt.plot([0, 0], [0, 0], 'g--', label='Hub communication range')

    # Axes limits & legend
    plt.xlim(0, vspace_x)
    plt.ylim(0, vspace_y)
    plt.legend(loc="upper right", facecolor='white', framealpha=1)

    plt.savefig('star.png', dpi=300)
    # plt.show()


if __name__ == '__main__':
    main()
