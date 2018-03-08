// Copyright WRLD Ltd (2018-), All Rights Reserved

package com.eegeo.searchproviders;

public class SearchProvidersJniMethods
{
    public static native void search(long nativeCallerPointer, String searchQuery);
    public static native void searchRefresh(long nativeCallerPointer, String searchQuery,
                                            boolean isTag, boolean tryInterior,
                                            double latitude, double longitude, double altitude,
                                            float radius);
    public static native void cancel(long nativeCallerPointer);
}
