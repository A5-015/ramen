#!/usr/bin/env python

import random
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
draw_node_range = False

draw_hubs = True
draw_hub_range = True


def main():

    draw_static('star')

    # draw_mesh()
    #
    # # update setting for the star network
    # global node_count, draw_node_range
    # node_count = 300
    # draw_node_range = False
    # draw_star()


def draw_static(type):
    nodes = {'x': [673, 1300, 1449, 1142, 1261, 1329, 1319, 710, 1083, 1202, 1325, 791,  982, 528],
             'y': [959,  624,  413, 1093, 1454,  612, 1405, 830,  777, 1363, 1172, 601, 1044, 583]}

    hubs = {'x': [660, 1260, 1050, 1310],
            'y': [750, 1360, 920, 500]}

    # (x0, x1), (y0, y1)
    mesh_edges = [[(528, 791), (583, 601)],
                  [(791, 710), (601, 830)],
                  [(528, 710), (583, 830)],
                  [(710, 673), (830, 959)],
                  [(528, 673), (583, 959)],
                  [(791, 1083), (601, 777)],
                  [(1083, 982), (777, 1044)],
                  [(673, 982), (959, 1044)],
                  [(1142, 982), (1093, 1044)],
                  [(1083, 1142), (777, 1093)],
                  [(1142, 1325), (1093, 1172)],
                  [(1083, 1300), (777, 624)],
                  [(1300, 1329), (624, 612)],
                  [(1329, 1449), (612, 413)],
                  [(1300, 1449), (624, 413)],
                  [(1325, 1261), (1172, 1454)],
                  [(1202, 1325), (1363, 1172)],
                  [(1261, 1319), (1454, 1405)],
                  [(1319, 1325), (1405, 1172)],
                  [(1261, 1202), (1454, 1363)],
                  [(1202, 1142), (1363, 1093)],
                  [(982, 710), (1044, 830)],
                  [(1083, 1329), (777, 612)]]

    star_edges = [[(528, 660), (583, 750)],
                  [(791, 660), (601, 750)],
                  [(710, 660), (830, 750)],
                  [(673, 660), (959, 750)],
                  [(1083, 1050), (777, 920)],
                  [(982, 1050), (1044, 920)],
                  [(1142, 1050), (1093, 920)],
                  [(1325, 1260), (1172, 1360)],
                  [(1202, 1260), (1363, 1360)],
                  [(1319, 1260), (1405, 1360)],
                  [(1261, 1260), (1454, 1360)],
                  [(1449, 1310), (413, 500)],
                  [(1329, 1310), (612, 500)],
                  [(1300, 1310), (624, 500)]]

    fig, ax = plt.subplots()

    # Place the nodes
    ax.scatter(nodes['x'], nodes['y'], color='b', s=30, label='Node')

    # Place the hubs
    if type is 'star':
        ax.scatter(hubs['x'], hubs['y'], color='g', s=60, label='Hub')

    # Node ranges
    if draw_node_range:
        plt.plot([0, 0], [0, 0], 'r--', label='Node communication range')
        for i in range(len(nodes['x'])):
            circle = plt.Circle((nodes['x'][i], nodes['y'][i]),
                                radius=350,
                                color='r', fill=False, linestyle='--')
            ax.add_artist(circle)

    # Hub ranges
    if draw_hub_range and (type is 'star'):
        plt.plot([0, 0], [0, 0], 'g--', label='Hub communication range')
        for i in range(len(hubs['x'])):
            circle = plt.Circle((hubs['x'][i], hubs['y'][i]),
                                radius=250,
                                color='g', fill=False, linestyle='--')
            ax.add_artist(circle)

    # Mesh node edges
    if draw_edges and (type is 'mesh'):
        plt.plot([0, 0], [0, 0], 'k-', label='Connections between nodes')
        for edge in mesh_edges:
            plt.plot(edge[0], edge[1], 'k-', zorder=0)

    # Star node edges
    if draw_edges and (type is 'star'):
        plt.plot([0, 0], [0, 0], 'k-', label='Connections between nodes')
        for edge in star_edges:
            plt.plot(edge[0], edge[1], 'k-', zorder=0)

    # Axes limits & legend
    plt.xlim(0, 2000)
    plt.ylim(0, 2000)
    plt.legend(loc="upper left", facecolor='white', framealpha=1)

    plt.savefig('static.png', dpi=300)
    # plt.show()


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
