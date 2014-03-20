// export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
#include <stdio.h>
#include <glib.h>
#include <CL/cl.h>

#include <ufo/ufo.h>

#define M_PI    3.14159265358979323846  /* pi */

#define SAMPLE 0

#if SAMPLE == 0
  #define IN_FILE_NAME "../../data/*.edf.tiff"
#endif

int main(int n_args, char *argv[])
{
  
  GError *error = NULL;

  UfoTaskGraph *ufo_task_graph;
  UfoScheduler *ufo_scheduler;
  UfoPluginManager *ufo_plugin_manager;

  UfoNode *reader;
  UfoNode *cut_sinogram;
  UfoNode *zeropadding;
  UfoNode *fft;
  UfoNode *dfi_sinc;
  UfoNode *swap_quadrants;
  UfoNode *ifft;
  UfoNode *swap_quadrants_inverse;
  UfoNode *region_of_interest;
  UfoNode *backproject;
  UfoNode *filter;
  //UfoNode *writer;
  g_type_init ();

  ufo_task_graph = ufo_task_graph_new();

  ufo_plugin_manager = ufo_plugin_manager_new (NULL);

  ufo_scheduler = ufo_scheduler_new (NULL, NULL);
  g_object_set (G_OBJECT (ufo_scheduler),
                "enable-tracing", TRUE,
                NULL);
  
  backproject = ufo_plugin_manager_get_task (ufo_plugin_manager, "backproject", &error);
  g_object_set (G_OBJECT (backproject),
                "axis-pos", 928.0f,
                NULL);

  reader = ufo_plugin_manager_get_task (ufo_plugin_manager, "reader", &error);
  g_object_set (G_OBJECT (reader),
                "path", IN_FILE_NAME,
                NULL);

  cut_sinogram = ufo_plugin_manager_get_task (ufo_plugin_manager, "cut_sinogram", &error);
  g_object_set (G_OBJECT(cut_sinogram),
                "center-of-rotation", 928.0f,
                NULL);

  zeropadding = ufo_plugin_manager_get_task (ufo_plugin_manager, "zeropadding", &error);
  g_object_set (G_OBJECT(zeropadding),
                "oversampling", 2,
                NULL);

  fft = ufo_plugin_manager_get_task (ufo_plugin_manager, "fft", &error);
  g_object_set (G_OBJECT(fft),
                NULL);

  filter = ufo_plugin_manager_get_task (ufo_plugin_manager, "filter", &error);
  g_object_set (G_OBJECT(filter),
                NULL);
  
  dfi_sinc = ufo_plugin_manager_get_task (ufo_plugin_manager, "dfi_sinc", &error);
  g_object_set (G_OBJECT(dfi_sinc),
                "kernel-size", 7,
		"number-presampled-values", 2047,
		"roi-size", 1696,
                NULL);

  swap_quadrants = ufo_plugin_manager_get_task (ufo_plugin_manager, "swap_quadrants", &error);
  g_object_set (G_OBJECT(swap_quadrants),
                NULL);

  ifft = ufo_plugin_manager_get_task (ufo_plugin_manager, "ifft", &error);
  g_object_set (G_OBJECT(ifft),
                NULL);

  swap_quadrants_inverse = ufo_plugin_manager_get_task (ufo_plugin_manager, "swap_quadrants", &error);
  g_object_set (G_OBJECT(swap_quadrants_inverse),
                NULL);


  region_of_interest = ufo_plugin_manager_get_task (ufo_plugin_manager, "region_of_interest", &error);
  g_object_set (G_OBJECT(region_of_interest),
		"x", 368,
		"y", 538,
		"width", 1311,
		"height", 1331,
                NULL);
  /*
  writer = ufo_plugin_manager_get_task (ufo_plugin_manager, "writer", &error);
  g_object_set (G_OBJECT (writer),
                "filename", "out-%05i.tif",
                NULL);
  */ 
  ufo_graph_connect_nodes (ufo_task_graph, reader, fft, NULL);
  ufo_graph_connect_nodes (ufo_task_graph, fft, filter, NULL);
  ufo_graph_connect_nodes (ufo_task_graph, filter, ifft, NULL);
  ufo_graph_connect_nodes (ufo_task_graph, ifft, backproject, NULL);
  //ufo_graph_connect_nodes (ufo_task_graph, backproject, writer, NULL);
  
  ufo_scheduler_run (ufo_scheduler, ufo_task_graph, &error);
  if (error) printf("\nError: Run was unsuccessful: %s\n", error->message);

  g_object_unref (ufo_task_graph);
  g_object_unref (ufo_scheduler);
  g_object_unref (ufo_plugin_manager);

  //g_object_unref (writer);
  g_object_unref (cut_sinogram);
  g_object_unref (zeropadding);
  g_object_unref (fft);
  g_object_unref (dfi_sinc);
  g_object_unref (swap_quadrants);
  g_object_unref (ifft);
  g_object_unref (swap_quadrants_inverse);
  g_object_unref (region_of_interest);
  g_object_unref (reader);

  return 0;
}
